/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2012 Thomas Trocha

    Contributor(s): non yet.
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
#include "gkGhost.h"
#include "gkDynamicsWorld.h"
#include "gkGameObject.h"
#include "gkScene.h"
#include "gkEntity.h"
#include "gkMesh.h"
#include "gkRigidBody.h"
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletDynamics/Character/btKinematicCharacterController.h"

#include "BulletCollision/BroadphaseCollision/btOverlappingPairCache.h"

#include "BulletCollision/BroadphaseCollision/btBroadphaseProxy.h"

#include "BulletCollision/BroadphaseCollision/btCollisionAlgorithm.h"

#include "BulletCollision/NarrowPhaseCollision/btPersistentManifold.h"

#include "LinearMath/btVector3.h"

#include "gkPhysicsController.h"

#include "gkContactTest.h"

#include "gkValue.h"
gkGhost::gkGhost(gkGameObject* object, gkDynamicsWorld* owner)
	: gkPhysicsController(object, owner)
{
}




gkGhost::~gkGhost()
{
}




void gkGhost::create(void)
{
	if (m_collisionObject)
		return;


	GK_ASSERT(m_object && m_object->isInstanced() && m_object->isInActiveLayer());

	// use the most up to date transform.
	const gkTransformState& trans = m_object->getWorldTransformState();


	createShape();


	if (!m_shape)
		return;


	m_collisionObject = new btPairCachingGhostObject();
	m_collisionObject->setUserPointer(this);
//	m_collisionObject->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
	m_collisionObject->setCollisionShape(m_shape);
	m_collisionObject->setWorldTransform(trans.toTransform());
	btDynamicsWorld* dyn = getOwner();
	dyn->addCollisionObject(m_collisionObject,m_props.m_colGroupMask,m_props.m_colMask);
	dyn->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());



}







void gkGhost::destroy(void)
{
	GK_ASSERT(m_object);

	btDynamicsWorld* dyn = getOwner();


	if (m_collisionObject)
	{
		// intertwine
		m_collisionObject->setUserPointer(0);

		if (!m_suspend)
			dyn->removeCollisionObject(m_collisionObject);

		delete m_shape;
		m_shape = 0;

		delete m_collisionObject;

		m_collisionObject = 0;
	}

}

// some try to get ghost working, since they stopped(?) for a while!?!?? I called this check-method from gkDynamicWorld's substep.
// I leave it in since here is still some stuff going on, where I'm not sure if that is needed someday. Since usually a ghost
// should handle its overlapping pairs its own, but for some reason our current mechanism seems to work as well (need some digging)

void gkGhost::check()
{
//	btTransform& trans = getCollisionObject()->getWorldTransform();
//	gkLogger::write("ORIGIN:"+ gkToString(gkMathUtils::get(trans.getOrigin()))+" obj:"+gkToString(getObject()->getWorldPosition()));
//	gkAllContactResultCallback exec;
//	getOwner()->contactTest(getCollisionObject(),exec);
//	if (exec.hasHit())
//	{
//		gkLogger::write("HAS HIT!");
//		utArray<const btCollisionObject*>::Iterator iter(exec.m_contactObjects);
//		while (iter.hasMoreElements())
//		{
//			m_localContacts.reserve(1);
//			gkPhysicsController* ob = gkPhysicsController::castController(iter.peekNext());
//			gkContactInfo cinf;
//			cinf.collider = ob;
//			m_localContacts.push_back(cinf);
//			gkLogger::write(this->getObject()->getName() + " HITS WITH:"+ob->getObject()->getName());
//			iter.next();
//		}
//	}
//	else
//	{
//		int a=0;
//		// NOT HIT :(
//	}

	//	m_localContacts.clear();
//	btBroadphasePairArray& pairArray =
//	    static_cast<btPairCachingGhostObject*>(m_collisionObject)->getOverlappingPairCache()->getOverlappingPairArray();
//	int numPairs = pairArray.size();
//
//	for (int i = 0; i < numPairs; ++i)
//	{
//	    manifoldArray.clear();
//
//	    const btBroadphasePair& pair = pairArray[i];
//
//	    btBroadphasePair* collisionPair =
//	        getOwner()->getPairCache()->findPair(
//	            pair.m_pProxy0,pair.m_pProxy1);
//
//	    if (!collisionPair) continue;
//
//	    if (collisionPair->m_algorithm)
//	        collisionPair->m_algorithm->getAllContactManifolds(manifoldArray);
//
//	    for (int j=0;j<manifoldArray.size();j++)
//	    {
//	        btPersistentManifold* manifold = manifoldArray[j];
//
//	        bool isFirstBody = manifold->getBody0() == m_collisionObject;
//
//	        btScalar direction = isFirstBody ? btScalar(-1.0) : btScalar(1.0);
//
//	        gkPhysicsController* colA = castController(manifold->getBody0());
//			gkPhysicsController* colB = castController(manifold->getBody1());
//
//			gkPhysicsController* collider = colB;
//
//			if (collider == this)
//			{
//				collider = colA;
//			}
//
//	        for (int p = 0; p < manifold->getNumContacts(); ++p)
//	        {
//	            const btManifoldPoint&pt = manifold->getContactPoint(p);
//
//	            if (pt.getDistance() < 0.f)
//	            {
//	                const btVector3& ptA = pt.getPositionWorldOnA();
//	                const btVector3& ptB = pt.getPositionWorldOnB();
//	                const btVector3& normalOnB = pt.m_normalWorldOnB;
//
//	                m_localContacts.reserve(1);
//
//					gkContactInfo cinf;
//					cinf.collider = collider;
//					m_localContacts.push_back(cinf);
//	                // handle collisions here
//					break;
//	            }
//	        }
//	    }
//	}


	// Here we must refresh the overlapping paircache as the penetrating movement itself or the
	// previous recovery iteration might have used setWorldTransform and pushed us into an object
	// that is not in the previous cache contents from the last timestep, as will happen if we
	// are pushed into a new AABB overlap. Unhandled this means the next convex sweep gets stuck.
	//
	// Do this by calling the broadphase's setAabb with the moved AABB, this will update the broadphase
	// paircache and the ghostobject's internal paircache at the same time.    /BW
//
//	btPairCachingGhostObject* m_ghostObject = static_cast<btPairCachingGhostObject*>(m_collisionObject);
//
//	btVector3 minAabb, maxAabb;
//	m_shape->getAabb(m_ghostObject->getWorldTransform(), minAabb,maxAabb);
//	getOwner()->getBroadphase()->setAabb(m_ghostObject->getBroadphaseHandle(),
//						 minAabb,
//						 maxAabb,
//						 getOwner()->getDispatcher());
//
//	bool penetration = false;
//
//	getOwner()->getDispatcher()->dispatchAllCollisionPairs(m_ghostObject->getOverlappingPairCache(), getOwner()->getDispatchInfo(), getOwner()->getDispatcher());
//
//	btVector3 m_currentPosition = m_ghostObject->getWorldTransform().getOrigin();
//
//	btScalar maxPen = btScalar(0.0);
//	for (int i = 0; i < m_ghostObject->getOverlappingPairCache()->getNumOverlappingPairs(); i++)
//	{
//		manifoldArray.resize(0);
//
//		btBroadphasePair* collisionPair = &m_ghostObject->getOverlappingPairCache()->getOverlappingPairArray()[i];
//
//		if (collisionPair->m_algorithm)
//			collisionPair->m_algorithm->getAllContactManifolds(manifoldArray);
//
//
//		for (int j=0;j<manifoldArray.size();j++)
//		{
//			btPersistentManifold* manifold = manifoldArray[j];
//			btScalar directionSign = manifold->getBody0() == m_ghostObject ? btScalar(-1.0) : btScalar(1.0);
//			for (int p=0;p<manifold->getNumContacts();p++)
//			{
//				const btManifoldPoint&pt = manifold->getContactPoint(p);
//
//				btScalar dist = pt.getDistance();
//
//				if (dist < 0.0)
//				{
//					if (dist < maxPen)
//					{
//						maxPen = dist;
//						btVector3 touchingNormal = pt.m_normalWorldOnB * directionSign;//??
//						int a=0;
//
//					}
//					m_currentPosition += pt.m_normalWorldOnB * directionSign * dist * btScalar(0.2);
//					penetration = true;
//				} else {
//					//printf("touching %f\n", dist);
//				}
//			}
//
//			//manifold->clearManifold();
//		}
//	}
//	btTransform newTrans = m_ghostObject->getWorldTransform();
//	newTrans.setOrigin(m_currentPosition);
//	m_ghostObject->setWorldTransform(newTrans);
//	printf("m_touchingNormal = %f,%f,%f\n",m_touchingNormal[0],m_touchingNormal[1],m_touchingNormal[2]);

//
//
}



void gkGhost::setWorldTransform(const btTransform& worldTrans)
{
	if (!m_collisionObject)
		return;

	gkPhysicsController::setTransform(worldTrans);

}


void gkGhost::_handleManifold(btPersistentManifold* manifold)
{
	gkPhysicsController* colA = castController(manifold->getBody0());
	gkPhysicsController* colB = castController(manifold->getBody1());

	gkPhysicsController* collider = colB;

	gkScalar direction = -1;
	if (collider == this)
	{
		collider = colA;
		direction = 1;
	}

	btTransform& trans = getCollisionObject()->getWorldTransform();


	for (int p = 0; p < manifold->getNumContacts(); ++p)
	{
		const btManifoldPoint&pt = manifold->getContactPoint(p);

		if (pt.getDistance() < 0.f)
		{
			const btVector3& ptA = pt.getPositionWorldOnA();
			const btVector3& ptB = pt.getPositionWorldOnB();
			const btVector3& normalOnB = pt.m_normalWorldOnB;

			m_localContacts.reserve(1);

			gkContactInfo cinf;
			cinf.collider = collider;
			m_localContacts.push_back(cinf);
			return;
			// handle collisions here
		}
	}

}






