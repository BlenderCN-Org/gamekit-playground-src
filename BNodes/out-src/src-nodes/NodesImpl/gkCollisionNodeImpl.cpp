/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkCollisionNodeImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

#include "Physics/gkPhysicsController.h"
#include "Physics/gkDynamicsWorld.h"
#include "gkGameObject.h"

#include "gkScene.h"

#include "gkEngine.h"
gkCollisionNodeImpl::gkCollisionNodeImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkCollisionNodeAbstract(parent,id,name),m_object(0)
{
	// setting the socketObject actively will prevent that the socket-object is set to the attached one
	getOUT_COLLIDED_OBJ()->setSocketObject(0);
}

gkCollisionNodeImpl::~gkCollisionNodeImpl()
{}

void gkCollisionNodeImpl::_initialize()
{
	gkScene* pScene = gkEngine::getSingleton().getActiveScene();

	pScene->getDynamicsWorld()->EnableContacts(true);

	getOUT_HAS_COLLIDED()->setValue(false);
	getOUT_COLLIDED_OBJ()->setValue("");

	getOUT_NOT_HAS_COLLIDED()->setValue(true);
}

bool gkCollisionNodeImpl::_evaluate(gkScalar tick)
{
	if (!getIN_ENABLED()->getValue())
	{
		return false;
	}
	gkGameObject* pObj = getIN_TARGET()->isConnected()
						? getIN_TARGET(true)->getGameObject()
						: getAttachedObject();

	m_object = 0;

	if (pObj && pObj->isInstanced())
	{
		m_object = pObj->getPhysicsController();
	}

	return m_object != 0;
}
void gkCollisionNodeImpl::_update(gkScalar tick)
{
	if (getIN_ENABLED())
	{
//		m_object->enableContactProcessing(true);

		if (getIN_COLLIDES_WITH()->isConnected())
		{
			gkGameObject* collidesWithObj = getIN_COLLIDES_WITH()->getGameObject();
			if (collidesWithObj)
			{
				m_object->enableContactProcessing(true);

				gkContactInfo c;

				if (m_object->collidesWith(collidesWithObj, &c))
				{
					gkGameObject* collisionObj = c.collider->getObject();
					if (collisionObj)
					{
						getOUT_CONTACT_POSITION()->setValue(gkVector3(c.point.getPositionWorldOnA()));
						getOUT_COLLIDED_OBJ()->getVar()->setValue(collisionObj->getName());
						getOUT_COLLIDED_OBJ()->setSocketObject(collisionObj);

						getOUT_HAS_COLLIDED()->setValue(true);
						getOUT_NOT_HAS_COLLIDED()->setValue(false);
						return;
					}
				}
			}
			getOUT_HAS_COLLIDED()->setValue(false);
			getOUT_COLLIDED_OBJ()->setSocketObject(0);
			getOUT_COLLIDED_OBJ()->setValue("");
			getOUT_NOT_HAS_COLLIDED()->setValue(true);

		}
		else
		{
			m_object->sensorCollides(getPropPROPERTY(), "", false, false,&m_colObjList);
			if (m_colObjList.size() > 0) // some collisions?
			{
				getOUT_HAS_COLLIDED()->setValue(true);
				getOUT_NOT_HAS_COLLIDED()->setValue(false);

				// TODO: This is silly
				gkString result("");
				CollisionObjects::Iterator iter(m_colObjList);
				while (iter.hasMoreElements())
				{
					gkGameObject* colObj = iter.getNext();
					if (result.empty())
					{
						result = colObj->getName();
						getOUT_COLLIDED_OBJ()->setSocketObject(colObj);
					}

				}

				if (m_colObjList.size()==0)
					getOUT_COLLIDED_OBJ()->setSocketObject(0);


				getOUT_COLLIDED_OBJ()->setValue(result);

				getOUT_CONTACT_POSITION()->setValue(gkVector3(0,0,0));
			}
			else
			{
				getOUT_HAS_COLLIDED()->setValue(false);
				getOUT_COLLIDED_OBJ()->setValue("");
				getOUT_NOT_HAS_COLLIDED()->setValue(true);
			}
		}
	}
	else
	{
		m_object->enableContactProcessing(false);
	}
}

