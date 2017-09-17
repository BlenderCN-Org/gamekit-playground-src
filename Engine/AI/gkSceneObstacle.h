/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2010 Nestor Silveira.

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
#ifndef _gkSceneObstacle_h_
#define _gkSceneObstacle_h_

#include "Obstacle.h"

#include "Physics/gkSweptTest.h"
class gkSceneRayObstacle : public OpenSteer::Obstacle
{
public:
	gkSceneRayObstacle(gkScalar howFarCanSee) : m_howFarCanSee(howFarCanSee), m_lastDirection(0) {}

	~gkSceneRayObstacle() {}

	void findIntersectionWithVehiclePath (const OpenSteer::AbstractVehicle& vehicle, OpenSteer::AbstractObstacle::PathIntersection& pi) const;

private:
	gkScalar m_howFarCanSee;

	mutable gkScalar m_lastDirection;
};

class gkSceneSweptObstacle : public OpenSteer::Obstacle
{
public:
	gkSceneSweptObstacle(gkScalar howFarCanSee,gkScalar radius,int filterGroup=btBroadphaseProxy::AllFilter,int filterMask=btBroadphaseProxy::AllFilter)
		: m_howFarCanSee(howFarCanSee),m_radius(radius),m_lastDirection(0),m_filterGroup(filterGroup),m_filterMask(filterMask),m_isPositiveList(false) {}

	~gkSceneSweptObstacle() {}

	void setPositiveList(bool isPositiveList) { m_isPositiveList = isPositiveList; }

	void findIntersectionWithVehiclePath (const OpenSteer::AbstractVehicle& vehicle, OpenSteer::AbstractObstacle::PathIntersection& pi) const;

	void ignoreObject(gkGameObject* gobj);
	void reincludeObject(gkGameObject* gobj);

private:

	gkScalar m_howFarCanSee;
	gkScalar m_radius;
	mutable gkScalar m_lastDirection;
	int m_filterGroup;
	int m_filterMask;
	bool m_isPositiveList;

	gkSweptTest::AVOID_LIST m_avoidList;
};


#endif//_gkSceneObstacle_h_
