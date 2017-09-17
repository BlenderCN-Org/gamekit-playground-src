/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkCameraNodeImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

#include "Physics/gkSweptTest.h"

#include "OgreRay.h"
gkCameraNodeImpl::gkCameraNodeImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkCameraNodeAbstract(parent,id,name),
	  m_center(gkVector3::ZERO),
	  m_oldCenter(gkVector3::ZERO),
	  m_target(0),
	  m_centerObj(0),
	  m_rollNode(gkQuaternion::IDENTITY),
	  m_pitchNode(gkQuaternion::IDENTITY),
	  m_idealRadius(0),
	  m_radiusIdealIsSet(false),
	  m_oldRadius(0),
	  m_oldRadiusIsSet(false)
{}

gkCameraNodeImpl::~gkCameraNodeImpl()
{}

void gkCameraNodeImpl::_initialize()
{
}

bool gkCameraNodeImpl::_evaluate(gkScalar tick)
{
	m_centerObj = getIN_CENTER_OBJ(true)->getGameObject();

	if (m_target != getIN_TARGET()->getGameObject())
	{
		m_radiusIdealIsSet = false;

		m_oldRadiusIsSet = false;

		m_target = getIN_TARGET()->getGameObject();

		m_rollNode = gkQuaternion(gkDegree(getIN_INITIAL_ROLL()->getValue()), gkVector3::UNIT_Z);

		m_pitchNode = gkQuaternion(gkDegree(getIN_INITIAL_PITCH()->getValue()), gkVector3::UNIT_X);

		m_oldCenter = m_center = getIN_CENTER_POSITION()->getValue();
	}

	bool update = getIN_UPDATE()->getValue();

	return update && m_centerObj && m_target && m_centerObj->isInstanced() && m_target->isInstanced();
}
void gkCameraNodeImpl::_update(gkScalar tick)
{
	float rx = -(getIN_REL_X()->getValue());

	gkQuaternion rollNode = m_rollNode * gkQuaternion(Ogre::Angle(-getIN_REL_X()->getValue()), gkVector3::UNIT_Z);

	gkScalar rollDegrees = rollNode.getRoll().valueDegrees();

	if (rollDegrees >= getIN_MIN_ROLL()->getValue() && rollDegrees <= getIN_MAX_ROLL()->getValue())
	{
		m_rollNode = rollNode;
	}

	gkQuaternion pitchNode = m_pitchNode * gkQuaternion(Ogre::Angle(-getIN_REL_Y()->getValue()), gkVector3::UNIT_X);

	gkScalar pitchDegrees = pitchNode.getPitch().valueDegrees();

	if (pitchDegrees >= getIN_MIN_PITCH()->getValue() && pitchDegrees <= getIN_MAX_PITCH()->getValue())
	{
		m_pitchNode = pitchNode;
	}

	m_target->setOrientation(m_rollNode * m_pitchNode);

	if (m_center != getIN_CENTER_POSITION()->getValue() && !getIN_CENTER_POSITION()->getValue().isNaN())
	{
		m_oldCenter = m_center;

		m_center = getIN_CENTER_POSITION()->getValue();
	}

	gkVector3 currentPosition = m_target->getPosition();

	Ogre::Vector3 dir;

	{
		gkVector3 newZPosition = currentPosition;

		if (getIN_REL_Z()->getValue())
		{
			newZPosition.z += newZPosition.z * getIN_REL_Z()->getValue() * 0.5;

			m_radiusIdealIsSet = false;
		}

		if (getIN_KEEP_DISTANCE()->getValue())
		{
			dir = m_oldCenter - newZPosition;

			m_oldCenter = m_center;
		}
		else
		{
			dir = m_center - newZPosition;
		}
	}

	gkScalar radius = dir.length();

	if (!m_radiusIdealIsSet)
	{
		m_idealRadius = radius;

		m_radiusIdealIsSet = true;
	}

	if (!m_oldRadiusIsSet)
	{
		m_oldRadius = radius;

		m_oldRadiusIsSet = true;
	}

	gkScalar stretch = (radius - m_idealRadius) * getIN_STIFNESS()->getValue();
	gkScalar damp = (radius - m_oldRadius) * getIN_DAMPING()->getValue();

	radius += -stretch * tick - damp;

	gkScalar minZ = getIN_MIN_Z()->getValue();
	gkScalar maxZ = getIN_MAX_Z()->getValue();

	if (radius < minZ)
	{
		radius = minZ;
	}
	else if (radius > maxZ)
	{
		radius = maxZ;
	}

	m_oldRadius = radius;

	calculateNewPosition(currentPosition, radius, tick);
//	m_rollNode.

	gkVector3 vec = gkEuler(m_rollNode).toVector3();
	getOUT_CURRENT_ROLL()->setValue(vec);
	getOUT_CURRENT_PITCH()->setValue(gkEuler(m_pitchNode).toVector3());
}

void gkCameraNodeImpl::calculateNewPosition(const gkVector3& currentPosition, gkScalar rayLength, gkScalar tick)
{
	gkVector3 oDir = gkVector3::NEGATIVE_UNIT_Z * rayLength;

	gkVector3 tmpPosition = m_center - m_target->getOrientation() * oDir;

	bool newPosSet = false;

	if (getIN_AVOID_BLOCKING()->getValue())
	{
		gkVector3 direction = tmpPosition - m_center;

		Ogre::Ray ray(m_center, direction);

		gkSweptTest::AVOID_LIST avoidList;
		avoidList.push_back(m_centerObj->getPhysicsController()->getCollisionObject());

		gkSweptTest sweptTest(avoidList);

		gkScalar blokingRadius = getIN_BLOCKING_RADIUS()->getValue();

		if (sweptTest.collides(ray, blokingRadius))
		{
			gkVector3 displacement = (sweptTest.getHitPoint() - currentPosition) * 0.9f;

			m_target->setPosition(currentPosition + (displacement + sweptTest.getSliding()) * tick);

			newPosSet = true;
		}
	}

	if (!newPosSet)
	{
		m_target->setPosition(tmpPosition);
	}
}


