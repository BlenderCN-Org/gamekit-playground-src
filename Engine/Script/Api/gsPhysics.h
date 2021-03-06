/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2010 Charlie C.

    Contributor(s): none yet.
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
#ifndef _gsPhysics_h_
#define _gsPhysics_h_

#include "gsCommon.h"
#include "gsMath.h"
#include "gsUtils.h"
#include "Script/Lua/gkLuaUtils.h"




class gsDynamicsWorld
{
private:
	gkDynamicsWorld* m_world;

public:
	gsDynamicsWorld(gkDynamicsWorld* dynamicsWorld);
	~gsDynamicsWorld();

	void exportBullet(const gkString& fileName);
	void setGravity(const gsVector3& gravity) {
		m_world->getBulletWorld()->setGravity(gkMathUtils::get(gravity));
	}

	gsVector3* getGravity() {
		return new gsVector3(gkMathUtils::get(m_world->getBulletWorld()->getGravity()));
	}
};

class gsVehicle {
public:
	gsVehicle(gkVehicle* veh) : m_vehicle(veh){}

	float getSpeed() {return m_vehicle->getCurrentSpeedKmHour();}
  	void setGaz(gkScalar ratio)                  { m_vehicle->setGaz(ratio); }
	void setBrake(gkScalar ratio)                { m_vehicle->setBrake(ratio); }
	void setSteer(gkScalar ratio)                { m_vehicle->setSteer(ratio); }
	void setGear(int gearNr)					 { m_vehicle->setCurrentGear(gearNr); }
	int getGear()								 { return m_vehicle->getCurrentGear(); }
private:
	gkVehicle* m_vehicle;
};

class gsRayTest
{
private:
	gkRayTest* m_ray;

public:
	gsRayTest(gsScene* scene=0);
	~gsRayTest();


	bool cast(gsRay& ray, const gkString& prop="", gsGameObject* excludeObject=0);

	gsVector3 getHitPoint(void);
	gsVector3 getHitNormal(void);
	gkGameObject* getHitObject(void);
	float getHitFraction(void);
};




class gsSweptTest
{
private:
	gkSweptTest* m_test;
	gkSweptTest::AVOID_LIST m_avoid;

public:
	gsSweptTest();
	gsSweptTest(gsArray<gsGameObject, gkGameObject> &avoid);
	~gsSweptTest();

	bool collides(const gsRay& ray, float rayRadius,int filterGroup=btBroadphaseProxy::AllFilter,int filterMask=btBroadphaseProxy::AllFilter);

	gsVector3 getHitPoint(void);
	gsVector3 getHitNormal(void);
	gsVector3 getReflection(void);
	gsVector3 getSliding(void);

	gkGameObject* getHitObject(void);
};

class gsCharacter
{
private:
    gkCharacter* m_character;
		
public:
    gsCharacter(gkCharacter* object);
    ~gsCharacter(void);

    gkGameObject* getObject(void);

    void setLinearVelocity(float forward, float backward, float rightward, float leftward, float timeInterval);    

    void setGravity(float gravity);

    void setRotation(const gsVector3& axis, float scalar);

    void setJumpSpeed(float scalar);
    void jump(void);

    bool isOnGround(void);
}; 

#endif //_gsPhysics_h_
