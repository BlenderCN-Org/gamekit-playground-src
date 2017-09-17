/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2010 Charlie C.

    Contributor(s): Nestor Silveira.
-------------------------------------------------------------------------------
  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------
*/
#include "gkMotionNode.h"
#include "gkEngine.h"
#include "gkGameObject.h"
#include "gkScene.h"


using namespace Ogre;



gkMotionNode::gkMotionNode(gkLogicTree* parent, size_t id, const gkString& name)
	:       gkLogicNode(parent, id,name), m_space(TRANSFORM_LOCAL),
	        m_keep(false), m_flag(0), m_otherName(""), m_current(0)
{
	ADD_ISOCK_VAR(UPDATE, false);
	ADD_ISOCK_VAR(X, 0);
	ADD_ISOCK_VAR(Y, 0);
	ADD_ISOCK_VAR(Z, 0);
	ADD_ISOCK_VAR(DAMPING, 0);
	ADD_ISOCK_VAR(OBJECT, "");

	ADD_OSOCK_VAR(RESULT, 0.9f);

	INIT_PROPERTY(MotionType,int,0);
	INIT_PROPERTY(Min,gkVector3,gkVector3(0,0,0));
	INIT_PROPERTY(Max,gkVector3,gkVector3(0,0,0));
}


bool gkMotionNode::evaluate(gkScalar tick)
{
	bool active = GET_SOCKET_VALUE(UPDATE);
	gkDebugScreen::printTo("gkMotionActive:"+gkToString(active));
	return active;
}

void gkMotionNode::initialize()
{
	updateOthername();
}

void gkMotionNode::updateOthername()
{
	gkString currentObjectName = GET_SOCKET_VALUE(OBJECT);
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

void gkMotionNode::applyConstraints(int lrs)
{
	// loc, rot, size
	CHECK_RETV(!m_current || lrs < 0 && lrs > 2);

	gkVector3 vec;
	bool doApply = false;

	gkVector3 min = m_Min->getValueVector3();
	gkVector3 max = m_Max->getValueVector3();

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


void gkMotionNode::applyObject(gkVector3& vec)
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


int gkMotionNode::getLRS()
{
	gkMotionTypes mT = _getMotionType();
	if (mT == MT_ROTATION)
		return 1;
	else if (mT == MT_TRANSLATION)
		return 0;

	else if (mT == MT_SCALE)
		return 2;

	else if (mT == MT_FORCE)
		return 0;

	else if (mT == MT_TORQUE)
		return 1;

	else if (mT == MT_LINV)
		return 0;

	else if (mT == MT_ANGV)
		return 1;

	return -1;
}


void gkMotionNode::update(gkScalar tick)
{
	gkMotionTypes mT = _getMotionType();
	CHECK_RETV(!m_object || mT == MT_NONE);

	if (GET_SOCKET(OBJECT)->isConnected())
		m_current = GET_SOCKET(OBJECT)->getGameObject();

	if (!m_current) m_current = getAttachedObject();

	gkVector3 vec;

	gkScalar d = 1.0 - GET_SOCKET_VALUE(DAMPING);
	gkScalar damp = gkAbs(d);
	damp = gkClampf(damp, 0.0, 1.0);

	vec.x = (GET_SOCKET_VALUE(X) * damp);
	vec.y = (GET_SOCKET_VALUE(Y) * damp);
	vec.z = (GET_SOCKET_VALUE(Z) * damp);

	if (mT == MT_ROTATION)
	{

		CHECK_RETV(gkFuzzyVec(vec));
		vec.x = gkClampf(vec.x, -MAX_ROT, MAX_ROT);
		vec.y = gkClampf(vec.y, -MAX_ROT, MAX_ROT);
		vec.z = gkClampf(vec.z, -MAX_ROT, MAX_ROT);

		if (m_other != 0) applyObject(vec);

		m_current->rotate(vec, m_space);
//		if (m_flag) applyConstraints(1);
	}

	else if (mT == MT_TRANSLATION)
	{

		CHECK_RETV(gkFuzzyVec(vec));
		vec.x = gkClampf(vec.x, -MAX_TRANSLATION, MAX_TRANSLATION);
		vec.y = gkClampf(vec.y, -MAX_TRANSLATION, MAX_TRANSLATION);
		vec.z = gkClampf(vec.z, -MAX_TRANSLATION, MAX_TRANSLATION);

		if (m_other != 0) applyObject(vec);

		m_current->translate(vec, m_space);

		if (m_flag) applyConstraints(0);
	}


	else if (mT == MT_SCALE)
	{
		CHECK_RETV(gkFuzzyVec(vec));
		vec.x = gkClampf(vec.x, -MAX_TRANSLATION, MAX_TRANSLATION);
		vec.y = gkClampf(vec.y, -MAX_TRANSLATION, MAX_TRANSLATION);
		vec.z = gkClampf(vec.z, -MAX_TRANSLATION, MAX_TRANSLATION);

		if (m_other != 0) applyObject(vec);

		m_current->scale(vec);

		if (m_flag) applyConstraints(2);
	}


	else if (mT == MT_FORCE)
	{
		vec.x = gkClampf(vec.x, -MAX_TRANSLATION, MAX_TRANSLATION);
		vec.y = gkClampf(vec.y, -MAX_TRANSLATION, MAX_TRANSLATION);
		vec.z = gkClampf(vec.z, -MAX_TRANSLATION, MAX_TRANSLATION);

		if (m_other != 0) applyObject(vec);
		m_current->applyForce(vec, m_space);
		if (m_flag) applyConstraints(0);
	}

	else if (mT == MT_TORQUE)
	{
		vec.x = gkClampf(vec.x, -MAX_TRANSLATION, MAX_TRANSLATION);
		vec.y = gkClampf(vec.y, -MAX_TRANSLATION, MAX_TRANSLATION);
		vec.z = gkClampf(vec.z, -MAX_TRANSLATION, MAX_TRANSLATION);

		if (m_other != 0) applyObject(vec);

		m_current->applyTorque(vec, m_space);
		if (m_flag) applyConstraints(1);
	}


	else if (mT == MT_LINV)
	{
		vec.x = gkClampf(vec.x, -MAX_TRANSLATION, MAX_TRANSLATION);
		vec.y = gkClampf(vec.y, -MAX_TRANSLATION, MAX_TRANSLATION);
		vec.z = gkClampf(vec.z, -MAX_TRANSLATION, MAX_TRANSLATION);

		if (m_keep)
		{
			gkVector3 ovel = m_current->getLinearVelocity();
			if (!GET_SOCKET(X)->isConnected()) vec.x = ovel.x;
			if (!GET_SOCKET(Y)->isConnected()) vec.y = ovel.y;
			if (!GET_SOCKET(Z)->isConnected()) vec.z = ovel.z;
		}

		if (m_other != 0) applyObject(vec);

		m_current->setLinearVelocity(vec, m_space);
		if (m_flag) applyConstraints(0);
	}

	else if (mT == MT_ANGV)
	{
		vec.x = gkClampf(vec.x, -MAX_TRANSLATION, MAX_TRANSLATION);
		vec.y = gkClampf(vec.y, -MAX_TRANSLATION, MAX_TRANSLATION);
		vec.z = gkClampf(vec.z, -MAX_TRANSLATION, MAX_TRANSLATION);

		if (m_keep)
		{
			gkVector3 ovel = m_current->getAngularVelocity();
			if (!GET_SOCKET(X)->isConnected()) vec.x = ovel.x;
			if (!GET_SOCKET(Y)->isConnected()) vec.y = ovel.y;
			if (!GET_SOCKET(Z)->isConnected()) vec.z = ovel.z;
		}

		if (m_other != 0) applyObject(vec);

		m_current->setAngularVelocity(vec, m_space);
		if (m_flag) applyConstraints(1);
	}
}
