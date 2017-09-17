/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkMotionNodeImpl.h>
#include "Logic/gkLogicNode.h"

#include "Logic/gkLogicSocket.h"

#include "gkDebugScreen.h"

#include "gkScene.h"

#include "gkMathUtils.h"
gkMotionNodeImpl::gkMotionNodeImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkMotionNodeAbstract(parent,id,name), m_current(0), m_flag(0)
{
	setBucketNr((int)BUCKET_SETTER);
}

gkMotionNodeImpl::~gkMotionNodeImpl()
{}

bool gkMotionNodeImpl::_evaluate(gkScalar tick)
{
	bool active = getIN_UPDATE()->getValue();
	return active;
}

void gkMotionNodeImpl::_initialize()
{
	updateOthername();
}

void gkMotionNodeImpl::updateOthername()
{
	gkString currentObjectName = GET_SOCKET_VALUE(IN_OBJECT);
	if (!currentObjectName.empty() && m_otherName!=currentObjectName)
	{
		m_otherName = currentObjectName;
		// look at global scope
		if (!m_other)
		{
			if (m_object)
			{
				gkScene* scene = m_object->getOwner();
				GK_ASSERT(scene);
				m_other = scene->getObject(m_otherName);
				m_current = 0;
			}
		}
	}
}

void gkMotionNodeImpl::applyConstraints(int lrs)
{
	// loc, rot, size
	CHECK_RETV(!m_current || lrs < 0 && lrs > 2);

	gkVector3 vec;
	bool doApply = false;

	gkVector3 min = getPropMinVec();
	gkVector3 max = getPropMaxVec();

	if (lrs == 0)
		vec = m_current->getPosition();
	else if (lrs == 2)
		vec = m_current->getScale();
	else
	{
		vec = m_current->getRotation().toVector3();

		// apply limit
		min.x = gkClampf(min.x, -MAX_ROT, MAX_ROT);
		max.x = gkClampf(max.x, -MAX_ROT, MAX_ROT);

		min.y = gkClampf(min.y, -MAX_ROT, MAX_ROT);
		max.y = gkClampf(min.y, -MAX_ROT, MAX_ROT);

		min.z = gkClampf(min.z, -MAX_ROT, MAX_ROT);
		max.z = gkClampf(max.z, -MAX_ROT, MAX_ROT);
	}
	if (m_flag & 1)  // X
	{
		if (vec.x < min.x)
		{
			vec.x = min.x;
			doApply = true;
		}
		if (vec.x > max.x)
		{
			vec.x = max.x;
			doApply = true;
		}
	}
	if (m_flag & 2) // Y
	{
		if (vec.y < min.y)
		{
			vec.y = min.y;
			doApply = true;
		}
		if (vec.y > max.y)
		{
			vec.y = max.y;
			doApply = true;
		}
	}

	if (m_flag & 4) // Z
	{
		if (vec.z < min.z)
		{
			vec.z = min.z;
			doApply = true;
		}
		if (vec.z > max.z)
		{
			vec.z = max.z;
			doApply = true;
		}
	}


	if (doApply)
	{
		if (lrs == 0)
			m_current->setPosition(vec);
		else if (lrs == 1)
			m_current->setOrientation(vec);
		else
			m_current->setScale(vec);
	}
}


void gkMotionNodeImpl::applyObject(gkVector3& vec)
{
	CHECK_RETV(!m_other);

	int lrs = getLRS();

	if (lrs == 0)
		vec = m_other->getWorldOrientation() * vec;
	else if (lrs == 1)
	{
		gkQuaternion q = gkMathUtils::getQuatFromEuler(vec);
		gkQuaternion w = m_other->getWorldOrientation();

		q =  q * w * q * w.Inverse();
		vec = gkMathUtils::getEulerFromQuat(q);
	}
	else if (lrs == 2)
		vec = m_other->getWorldScale() * vec;
}


int gkMotionNodeImpl::getLRS()
{
	Motion mT = (Motion)getPropMotion();
	if (mT == Motion_rotation)
		return 1;
	else if (mT == Motion_translation)
		return 0;

	else if (mT == Motion_scale)
		return 2;

	else if (mT == Motion_force)
		return 0;

	else if (mT == Motion_torque)
		return 1;

	else if (mT == Motion_linvel)
		return 0;

	else if (mT == Motion_angvel)
		return 1;

	return -1;
}


void gkMotionNodeImpl::_update(gkScalar tick)
{
	Motion mT = (Motion)getPropMotion();
	CHECK_RETV(!m_object || mT == Motion_none);

	if (GET_SOCKET(IN_OBJECT)->isConnected())
		m_current = GET_SOCKET(IN_OBJECT)->getGameObject();

	if (!m_current) m_current = getAttachedObject();

	gkVector3 vec;

	gkScalar d = 1.0 - GET_SOCKET_VALUE(IN_DAMPING);
	gkScalar damp = gkAbs(d);
	damp = gkClampf(damp, 0.0, 1.0);

	vec.x = (GET_SOCKET_VALUE(IN_X) * damp);
	vec.y = (GET_SOCKET_VALUE(IN_Y) * damp);
	vec.z = (GET_SOCKET_VALUE(IN_Z) * damp);

	if (mT == Motion_rotation)
	{

		CHECK_RETV(gkFuzzyVec(vec));
		vec.x = gkClampf(vec.x, -MAX_ROT, MAX_ROT);
		vec.y = gkClampf(vec.y, -MAX_ROT, MAX_ROT);
		vec.z = gkClampf(vec.z, -MAX_ROT, MAX_ROT);

		if (m_other != 0) applyObject(vec);

		m_current->rotate(vec, getPropSpace());
//		if (m_flag) applyConstraints(1);
	}

	else if (mT == Motion_translation)
	{

		CHECK_RETV(gkFuzzyVec(vec));
		vec.x = gkClampf(vec.x, -MAX_TRANSLATION, MAX_TRANSLATION);
		vec.y = gkClampf(vec.y, -MAX_TRANSLATION, MAX_TRANSLATION);
		vec.z = gkClampf(vec.z, -MAX_TRANSLATION, MAX_TRANSLATION);

		if (m_other != 0) applyObject(vec);

		m_current->translate(vec, getPropSpace());

		if (m_flag) applyConstraints(0);
	}


	else if (mT == Motion_scale)
	{
		CHECK_RETV(gkFuzzyVec(vec));
		vec.x = gkClampf(vec.x, -MAX_TRANSLATION, MAX_TRANSLATION);
		vec.y = gkClampf(vec.y, -MAX_TRANSLATION, MAX_TRANSLATION);
		vec.z = gkClampf(vec.z, -MAX_TRANSLATION, MAX_TRANSLATION);

		if (m_other != 0) applyObject(vec);

		m_current->scale(vec);

		if (m_flag) applyConstraints(2);
	}


	else if (mT == Motion_force)
	{
		vec.x = gkClampf(vec.x, -MAX_TRANSLATION, MAX_TRANSLATION);
		vec.y = gkClampf(vec.y, -MAX_TRANSLATION, MAX_TRANSLATION);
		vec.z = gkClampf(vec.z, -MAX_TRANSLATION, MAX_TRANSLATION);

		if (m_other != 0) applyObject(vec);
		m_current->applyForce(vec, getPropSpace());
		if (m_flag) applyConstraints(0);
	}

	else if (mT == Motion_torque)
	{
		vec.x = gkClampf(vec.x, -MAX_TRANSLATION, MAX_TRANSLATION);
		vec.y = gkClampf(vec.y, -MAX_TRANSLATION, MAX_TRANSLATION);
		vec.z = gkClampf(vec.z, -MAX_TRANSLATION, MAX_TRANSLATION);

		if (m_other != 0) applyObject(vec);

		m_current->applyTorque(vec, getPropSpace());
		if (m_flag) applyConstraints(1);
	}


	else if (mT == Motion_linvel)
	{
		vec.x = gkClampf(vec.x, -MAX_TRANSLATION, MAX_TRANSLATION);
		vec.y = gkClampf(vec.y, -MAX_TRANSLATION, MAX_TRANSLATION);
		vec.z = gkClampf(vec.z, -MAX_TRANSLATION, MAX_TRANSLATION);

		gkVector3 ovel = m_current->getLinearVelocity();
		BoolArray keepXYZ = getPropKeepXYZ();
		if (keepXYZ[0])
			vec.x = ovel.x;
		else
			vec.x = getIN_X()->getValue();

		if (keepXYZ[1])
			vec.y = ovel.y;
		else
			vec.y = getIN_Y()->getValue();

		if (keepXYZ[2])
			vec.z = ovel.z;
		else
			vec.z = getIN_Z()->getValue();

//		if (m_other != 0)
//			applyObject(vec);

		m_current->setLinearVelocity(vec, getPropSpace());
		if (m_flag) applyConstraints(0);
	}

	else if (mT == Motion_angvel)
	{
		vec.x = gkClampf(vec.x, -MAX_TRANSLATION, MAX_TRANSLATION);
		vec.y = gkClampf(vec.y, -MAX_TRANSLATION, MAX_TRANSLATION);
		vec.z = gkClampf(vec.z, -MAX_TRANSLATION, MAX_TRANSLATION);

//		BoolArray keepXYZ = getPropKeepXYZ();
//		gkVector3 ovel = m_current->getAngularVelocity();
//
//		if (getPropKeep())
//		{
//			if (!GET_SOCKET(IN_X)->isConnected()) vec.x = ovel.x;
//			if (!GET_SOCKET(IN_Y)->isConnected()) vec.y = ovel.y;
//			if (!GET_SOCKET(IN_Z)->isConnected()) vec.z = ovel.z;
//		}

		if (m_other != 0) applyObject(vec);

		m_current->setAngularVelocity(vec, getPropSpace());
		if (m_flag) applyConstraints(1);
	}
	else if (mT == Motion_set_pos)
	{
		m_current->setPosition(gkVector3(vec));
	}
	else if (mT == Motion_set_rot)
	{
		m_current->setOrientation(gkEuler(vec));
	}
}

