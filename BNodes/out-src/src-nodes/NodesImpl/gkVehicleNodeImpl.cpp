/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkVehicleGearboxImpl.h>
#include <BNodes/out-src/src-nodes/NodesImpl/gkVehicleGearImpl.h>
#include <BNodes/out-src/src-nodes/NodesImpl/gkVehicleNodeImpl.h>
#include <BNodes/out-src/src-nodes/NodesImpl/gkVehicleWheelImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

#include "Physics/gkVehicle.h"

gkVehicleNodeImpl::gkVehicleNodeImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkVehicleNodeAbstract(parent,id,name), m_steer(0),m_vehicle(0)
{}

gkVehicleNodeImpl::~gkVehicleNodeImpl()
{}

void gkVehicleNodeImpl::_afterInit()
{
	createVehicle();
}

bool gkVehicleNodeImpl::_evaluate(gkScalar tick)
{
	if (!isValid()) {
		gkLogger::write("gkVehicle is not valid:"+getName());
		return false;
	}
	float steerSpeed = getIN_STEER_TIME(true)->getValue();
	bool front = getIN_FRONT(true)->getValue();
	bool rear = getIN_REAR(true)->getValue();
	bool left = getIN_LEFT(true)->getValue();
	bool right = getIN_RIGHT(true)->getValue();
	bool brake = false;


	if (!getIN_STEER_VALUE(true)->isConnected())
	{
		if (left)
			m_steer += tick / steerSpeed;
		else if (right)
			m_steer -= tick / steerSpeed;
		else
		{
			if (m_steer > 0)
			{
				m_steer -= tick / steerSpeed;
				m_steer = gkMaxf(m_steer, 0);
			}
			else
			{
				m_steer += tick / steerSpeed;
				m_steer = gkMinf(m_steer, 0);
			}
		}
	}
	else
	{
		m_steer = getIN_STEER_VALUE()->getValue();
	}

	m_steer = gkClampf(m_steer, -1.0f, 1.0f);

	if (m_vehicle)
	{
		if (getIN_GEAR_UP(true)->getValue())
			m_vehicle->shiftUp();
		else if (getIN_GEAR_DOWN(true)->getValue())
			m_vehicle->shiftDown();

		m_vehicle->setGaz(0.0);
		m_vehicle->setSteer(0.0);

		if (front)
		{
			if (gkAbs(m_vehicle->getCurrentSpeedKmHour()) < 1 && m_vehicle->getCurrentGear() == -1)
				m_vehicle->shiftUp();
			if (m_vehicle->getCurrentGear() != -1)
				m_vehicle->setGaz(1.0);
			else
				brake = true;
		}

		if (rear)
		{
			if (gkAbs(m_vehicle->getCurrentSpeedKmHour()) < 1 && m_vehicle->getCurrentGear() != -1)
				m_vehicle->shiftDown();
			if (m_vehicle->getCurrentGear() == -1)
				m_vehicle->setGaz(1.0);
			else
				brake = true;
		}

		if (brake)
			m_vehicle->setBrake(1.0);
		else
			m_vehicle->setBrake(0.0);

		m_vehicle->setSteer(m_steer);

		m_vehicle->setHandBrake(getIN_HAND_BRAKE(true)->getValue());


		getOUT_ZROT()->setValue(m_vehicle->getVelocityEulerZ());
		getOUT_KMH()->setValue((int)(m_vehicle->getCurrentSpeedKmHour() + 0.5));
		getOUT_GEAR()->setValue(m_vehicle->getCurrentGear());
		getOUT_RPM()->setValue(m_vehicle->getCurrentRpm());
	}

	bool update = getIN_UPDATE(true)->getValue();

	return update && m_vehicle;

}
void gkVehicleNodeImpl::_update(gkScalar tick)
{
	m_vehicle->tick(tick);
}

void gkVehicleNodeImpl::createVehicle()
{
	if (m_vehicle) {
		gkLogger::write("ERROR: you tried to create vehicle, but there is already one! IGNORING");
		return;
	}

	gkGameObject* chassis = getAttachedObject();

//	m_vehicle = chassis->getOwner()->getDynamicsWorld()->createVehicle(chassis);
	m_vehicle = new gkVehicle(chassis->getOwner());
	m_vehicle->setChassisObject(chassis);

	m_vehicle->setDriveTrain(gkVehicle::DT_ALLWHEEL);
	m_vehicle->setEngineTorque(getPropEngineTorque());
	m_vehicle->setBrakePower(getPropBreakPower());
	m_vehicle->setRearBrakeRatio(getPropRearBreakRatio());
	m_vehicle->setMaxSteeringAngle(getPropMaxSteeringAngle());
	m_vehicle->setRuptorRpm(getPropRuptorRpm());

	if (getIN_GEARBOX(true)->isConnected())
	{
		// todo
		gkVehicleGearboxImpl* gbNode = static_cast<gkVehicleGearboxImpl*>(getIN_GEARBOX()->getFrom()->getParent());

		bool automatic = gbNode->getPropautomatic();
		int numGears = gbNode->getOUT_GEARS(true)->getOutSocketAmount();
		float shiftTime = gbNode->getPropshiftTime();
		float reverseRatio = gbNode->getPropreverseRatio();

		gkGearBox* gearbox = new gkGearBox(automatic,numGears,shiftTime,reverseRatio);
		if (gbNode->isValid())
		{
			for (int i=0;i<numGears;i++)
			{
				gkVehicleGearImpl* gearNode = static_cast<gkVehicleGearImpl*>(gbNode->getOUT_GEARS()->getOutSocketAt(i)->getParent());
				int nr = gearNode->getPropgearNr();
				float ratio = gearNode->getPropratio();
				float rpmLow = gearNode->getProprpmLow();
				float rpmHigh = gearNode->getProprpmHigh();
				gearbox->setGearProperties(nr,ratio,rpmLow,rpmHigh);
			}
		}
		m_vehicle->setGearBox(gearbox);
	}
	else
	{
		if (getIN_GEARBOX()->getValue()) {
			m_vehicle->setDefaultGearBox();
		}
	}

	// wheels
	for (int i = 0;i< getOUT_WHEELS()->getOutSocketAmount();i++) {
		gkVehicleWheelImpl* wheelNode = static_cast<gkVehicleWheelImpl*>(getOUT_WHEELS()->getOutSocketAt(i)->getParent());

		gkString wheelObjName = wheelNode->getPropwheelObject();
		gkGameObject* wheelObj = chassis->isGroupInstance()
									?chassis->getGroupInstance()->getObject(wheelObjName)
									:chassis->getOwner()->getObject(wheelObjName);

		if (!wheelObj) {
			gkLogger::write("Couldn't find wheel:"+wheelObjName);
			m_isValid = false;
			return;
		}

		gkVector3 connectionPoint(wheelObj->getWorldPosition() - chassis->getWorldPosition());
//		gkVector3 connectionPoint(chassis->getWorldPosition() - wheelObj->getWorldPosition());

		gkVector3 vec = wheelObj->getAabb().getHalfSize();

		float radius = vec.z;
//		connectionPoint.z = 0.267;

		float suspensionTravelCm = wheelNode->getPropsuspensionTravelCm();
		float suspensionRestLength = wheelNode->getPropsuspensionRestLength();
		float suspensionStiffness = wheelNode->getPropsuspensionStiffness();
		float suspensionRelax = 0.6 * 2.0 * btSqrt(suspensionStiffness);
		float suspensionCompression = 0.6 * 2.0 * btSqrt(suspensionStiffness);
		float friction = wheelNode->getPropfriction();
		float rollInfluence = wheelNode->getProprollInfluence();

		bool isFront = wheelNode->getPropisFront();

		gkVector3 wheelAxis = wheelNode->getPropaxis();
//		gkVector3 wheelAxis(1,0,0);
		gkVector3 wheelDirection(0,0,-1);

		m_vehicle->addWheel(wheelObj, radius, connectionPoint, wheelDirection, wheelAxis, isFront,
		                    suspensionRestLength, suspensionStiffness, suspensionRelax, suspensionCompression,
		                    friction, rollInfluence, suspensionTravelCm);

	}
	m_vehicle->createVehicle();
;
}
