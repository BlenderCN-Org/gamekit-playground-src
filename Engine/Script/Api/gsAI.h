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
#ifndef _gsAI_h_
#define _gsAI_h_


#include "gsCommon.h"
#include "gsMath.h"
#include "gsUtils.h"
#include "Script/Lua/gkLuaUtils.h"

#include "gkSteeringObject.h"

#include "gkSceneObstacle.h"

#include "gkSteeringPathFollowing.h"

#include "AbstractVehicle.h"

#include "SimpleVehicle.h"

#include "gkRecast.h"

#include "gsCore.h"

#include "gkCurve.h"


#ifdef OGREKIT_COMPILE_RECAST
//# include "DetourCrowd.h"
// # include "DetourSceneCreator.h"
//# include "DetourCommon.h"
#endif

class gsWhenEvent
{
public:
	~gsWhenEvent();
	void when(gsFunction when);
	void when(gsSelf self, gsFunction when);
	void when(gsSelf self, gsFunction when, const gkString& stParam);
	void when(gsSelf self, gsFunction when, int intParam);
	void when(gsSelf self, gsFunction when, float floatParam);
	void when(gsSelf self, gsFunction when, bool boolParam);

	OGRE_KIT_WRAP_CLASS_COPY_CTOR(gsWhenEvent, gkFSM::Event, m_event);
};



class gsFSM
{
private:
	typedef utHashTable<utIntHashKey, class gsUpdateEvent*> EVENT;

	gkFSM*   m_fsm;
	int      m_curState;
	EVENT    m_events;

public:

	gsFSM();
	~gsFSM();


	void  update();
	void  setState(int state);
	int   getState();



	void  addStartTrigger(int state, gsSelf self, gsFunction trigger);
	void  addEndTrigger(int state, gsSelf self, gsFunction trigger);


	void  addEvent(int state, gsSelf self, gsFunction update);

	gsWhenEvent* addTransition(int from, int to);
	gsWhenEvent* addTransition(int from, int to, unsigned long ms);
	gsWhenEvent* addTransition(int from, int to, unsigned long ms, gsSelf self, gsFunction trigger);

};

#ifdef OGREKIT_COMPILE_OPENSTEER
class gsNavPath
{
public:
	gsNavPath() : mNavMeshScene(0)
	{
		mPath = new gkNavPath();
	}

	~gsNavPath() {
		delete mPath;
	}

	void addPoint(const gsVector3& vec) { mPath->addPoint(vec); }
	gsVector3 getPoint(int nr) { return gsVector3(mPath->getPoint(nr)); }
	int getPointAmount() { return mPath->getPointAmount(); }
	void setCyclic(bool isCyclic) { mPath->setCyclic(isCyclic);}
	bool isCyclic() { return mPath->isCyclic();}
	void createFromCurve(gsCurve* curve,float radius=1.0f,float ab=0.1f)
	{
		mPath->createFromCurve(static_cast<gkCurve*>(curve->get()),radius,ab);
	}

	void clear() {
		mPath->clear();
	}

	void show() {
		mPath->showPath();
	}

	gsVector3 getLast()
	{
		int pA = getPointAmount();
		if (pA>0)
			return getPoint(pA-1);
		else
			return gsVector3(0,0,0);
	}

#ifdef OGREKIT_COMPILE_RECAST
public:
	void createFromRecast(gsScene* scene,const gsVector3& from,const gsVector3& to,float radius=2.0f,const gsVector3& polyPickExt=gsVector3(2,4,2),int maxPolys=300)
	{
		if (!scene || !scene->getOwner()->hasNavMesh())
		{
			gkDebugScreen::printTo("ERROR: gkNavPath:createFromRecast has no valid navmesh. call scene:asyncCreateNavMesh() first and wait until it is made!");
			return;
		}


		PDT_NAV_MESH m_navMesh = scene->getOwner()->getNavMesh();

		get()->createFromRecast(m_navMesh,from,to,polyPickExt,maxPolys,radius);

	}







#endif


	gkNavPath* get() { return mPath; }
private:
	gkNavPath* mPath;
#ifdef OGREKIT_COMPILE_RECAST
	gkScene* mNavMeshScene;
#endif
};


class gsSteerObject;




class gsSteerGroup
{
public:
	gsSteerGroup(){};

	void add(gsSteerObject* obj);

	const OpenSteer::AVGroup& get() {
		return mGroup;
	}

	void clear();

	int getAmount() { return mGroup.size(); }
private:
	OpenSteer::AVGroup mGroup;
};

class gsSteerRayObstacle
{
public:
	gsSteerRayObstacle(float howFarCanSee=5.0f)
	{
		m_obstacle = new gkSceneRayObstacle(howFarCanSee);
	}

	~gsSteerRayObstacle() {
		delete m_obstacle;
	}

	gkSceneRayObstacle* get() { return m_obstacle;}

private:
	gkSceneRayObstacle* m_obstacle;
};

class gsSteerSweptObstacle
{
public:
	gsSteerSweptObstacle(float howFarCanSee=5.0f,float radius=1.0f,int filterGroup=btBroadphaseProxy::AllFilter,int filterMask=btBroadphaseProxy::AllFilter)
	{
		m_obstacle = new gkSceneSweptObstacle(howFarCanSee,radius,filterGroup,filterMask);
	}

	void ignoreObject(gsGameObject* gobj) {
		m_obstacle->ignoreObject(gobj->get());
	}

	void reincludeObject(gsGameObject* gobj) {
		m_obstacle->reincludeObject(gobj->get());
	}

	void setPositiveList(bool isPositiveList) {m_obstacle->setPositiveList(isPositiveList);}

	~gsSteerSweptObstacle() {
		delete m_obstacle;
	}

	gkSceneSweptObstacle* get() { return m_obstacle;}

private:
	gkSceneSweptObstacle* m_obstacle;
};

class gsSteerObstacleGroup
{
public:
	gsSteerObstacleGroup(){};

	void add(gsSteerSweptObstacle* obj);

	const OpenSteer::ObstacleGroup& get() {
		return mGroup;
	}

private:
	OpenSteer::ObstacleGroup mGroup;
};

class gsSteerPathFollowing
{
public:
	gsSteerPathFollowing(gsGameObject* gsobj,float maxSpeed=1.0f)
	{
		mSteerPathFollowing = new gkSteeringPathFollowing(gsobj->get(),maxSpeed,gkVector3(0,1,0),gkVector3(0,0,1),gkVector3(1,0,0));
	}
	~gsSteerPathFollowing() {
		delete mSteerPathFollowing;
	}

	void setSpeed(float s) {
		mSteerPathFollowing->setSpeed(s);
	}

	void setPath(gsNavPath* path)
	{
		mSteerPathFollowing->setNavPath(path->get());
	}

	void update(float dt) {
		mSteerPathFollowing->update(dt);
	}
private:
	gkSteeringPathFollowing* mSteerPathFollowing;
};

class MyVehicle
#ifndef SWIG
		: public OpenSteer::SimpleVehicle
#endif
{
public:
	MyVehicle() {
		OpenSteer::SimpleVehicle::reset();
	}
};

class gsSteerObject
{
public:
	gsSteerObject(gsGameObject* gsobj,float maxSpeed,float speed=0)
		: mSteerObj(gsobj->get(),3,gkVector3(0,1,0),gkVector3(0,0,1),gkVector3(1,0,0)), gobj(gsobj->get())
	{

//		mSteerObj.setPosition(OpenSteer::Vec3(gsobj->get()->getWorldPosition()));
		mSteerObj.setSpeed(speed);
		mSteerObj.setRadius(0.5f);
		mSteerObj.setMaxSpeed(maxSpeed);
	}

	OpenSteer::AbstractVehicle* get() { return &mSteerObj; }

	void applySteeringForce(const gsVector3& force,float dt)
	{
		mSteerObj.applySteeringForce(force,dt);
	}

	void applyBreakingForce(float rate, float dt)
	{
		mSteerObj.applyBrakingForce(rate,dt);
	}

	float radius() {
		return mSteerObj.radius();
	}

	void setRadius(float radius) {
		mSteerObj.setRadius(radius);
	}

	float speed(){
		return mSteerObj.speed();
	}

	void setSpeed(float speed) {
		mSteerObj.setSpeed(speed);
	}

	float maxSpeed(){
		return mSteerObj.maxSpeed();
	}

	void setMaxSpeed(float maxSpeed){
		mSteerObj.setMaxSpeed(maxSpeed);
	}

	gsVector3 forward() {
		return gsVector3(mSteerObj.forward());
	}
	gsVector3 side() {
		return gsVector3(mSteerObj.side());
	}
	gsVector3 up() {
		return gsVector3(mSteerObj.up());
	}
	gsVector3 predictedPosition(float time) {
		return gsVector3(mSteerObj.predictFuturePosition(time));
	}
	gsVector3 velocity() {
		return gsVector3(mSteerObj.velocity());
	}
	gsVector3 position() {
		return gsVector3(mSteerObj.position());
	}

//
//	gsQuaternion getRotationTo(const gsVector3& vec) {(0,0,0)
//		return gsQuaternion(m_steerUtility.getRotation(forward(),vec.normalisedCopy() ));
//	}


	gsVector3 steerForWander(float dt) {
		return gsVector3(mSteerObj.steerForWander(dt));
	}

	gsVector3 steerForPursuit(gsSteerObject* target,float maxPrediction=2){
		return gsVector3(mSteerObj.steerForPursuit(*target->get(),maxPrediction));
	}

	gsVector3 steerForSeparation(gsSteerGroup* group,float maxDistance=1,float maxCosAngle=0.5f){
		return gsVector3(mSteerObj.steerForSeparation(maxDistance,maxCosAngle,group->get()));
	}

	gsVector3 steerForSeek(const gsVector3& vec)
	{
		return gsVector3(mSteerObj.xxxsteerForSeek(OpenSteer::Vec3(vec)));
	}

//	gsVector3 steerTowardHeading(const gsVector3& vec)
//	{
//		return gsVector3(mSteerObj.steerTowardHeading(vec));
//	}

	gsVector3 steerForFlee(gsVector3 vec)
	{
		return gsVector3(mSteerObj.xxxsteerForFlee(OpenSteer::Vec3(vec)));
	}

	gsVector3 steerToAvoidNeighbours(gsSteerGroup* grp,float minTimeToCollision=5.0f) {
		return gsVector3(mSteerObj.steerToAvoidNeighbors(minTimeToCollision,grp->get()));
	}

	gsVector3 steerToAvoidObstacles(gsSteerObstacleGroup* grp,float minTimeToCollision=5.0f) {
		return gsVector3(mSteerObj.steerToAvoidObstacles(minTimeToCollision,grp->get()));
	}

	gsVector3 steerToAvoidObstacle(gsSteerSweptObstacle* obstacle,float minTimeToCollision=5.0f) {
		if (!obstacle){
			gkDebugScreen::printTo("ERROR:nil obstacle!!! ("+gobj->getName()+" )");
			return gsVector3::ZERO;
		}
		return gsVector3(mSteerObj.steerToAvoidObstacle(minTimeToCollision,*obstacle->get()));
	}

	gsVector3 steerForAlignment(gsSteerGroup* flock,float maxDistance=1.0f,float cosMaxAngle=0.5f)
	{
		return gsVector3(mSteerObj.steerForAlignment(maxDistance,cosMaxAngle,flock->get()));
	}

    // basic flocking
    gsVector3 steerToFlock (gsSteerGroup* flock)
    {
        // avoid obstacles if needed
        // XXX this should probably be moved elsewhere
        const float separationRadius =  5.0f;
        const float separationAngle  = -0.707f;
        const float separationWeight =  12.0f;

        const float alignmentRadius = 7.5f;
        const float alignmentAngle  = 0.7f;
        const float alignmentWeight = 8.0f;

        const float cohesionRadius = 9.0f;
        const float cohesionAngle  = -0.15f;
        const float cohesionWeight = 8.0f;


        // determine each of the three component behaviors of flocking
        const OpenSteer::Vec3 separation = mSteerObj.steerForSeparation (separationRadius,
                                                    separationAngle,
                                                    flock->get());
        const OpenSteer::Vec3 alignment  = mSteerObj.steerForAlignment  (alignmentRadius,
                                                    alignmentAngle,
													flock->get());
        const OpenSteer::Vec3 cohesion   = mSteerObj.steerForCohesion   (cohesionRadius,
                                                    cohesionAngle,
													flock->get());

        // apply weights to components (save in variables for annotation)
        const OpenSteer::Vec3 separationW = separation * separationWeight;
        const OpenSteer::Vec3 alignmentW = alignment * alignmentWeight;
        const OpenSteer::Vec3 cohesionW = cohesion * cohesionWeight;

        // annotation
        // const float s = 0.1;
        // annotationLine (position, position + (separationW * s), gRed);
        // annotationLine (position, position + (alignmentW  * s), gOrange);
        // annotationLine (position, position + (cohesionW   * s), gYellow);

        return separationW + alignmentW + cohesionW;
    }


	gsVector3 steerToFollowPath(int direction, float predictionTime, gsNavPath* path)
	{
		return mSteerObj.steerToFollowPath(direction,predictionTime,*path->get());
	}

	gsVector3 steerToStayOnPath(float predictionTime, gsNavPath* path)
	{
		return mSteerObj.steerToStayOnPath(predictionTime,*path->get());
	}

	gsVector3 steerForTargetSpeed(float speed)
	{
		return mSteerObj.steerForTargetSpeed(speed);
	}

//	gsVector3 steerToFollowPathLinear(int direction, float predictionTime, gsNavPath* path)
//	{
//		return mSteerObj.steerToFollowPathLinear(direction,predictionTime,*path->get());
//	}
//
//	void calculate(gsVector3& steering,float time)
//	{
//		mSteerObj.calcSteeringForce(steering,time,dir,rot);
//	}
//
//	gsVector3 getDirection() {
//		return gsVector3(dir);
//	}
//
//	gsQuaternion getRotation() {
//		return gsQuaternion(rot);
//	}
//
//	void doit(float dt) {
//
//	}
//
//	bool isInGoal() {
//		return mSteerObj.inGoal();
//	}
//
//	bool tick(float dt) {
//		return mSteerObj.update(dt);
//	}
private:
//	MyVehicle mSteerObj;
	gkSteeringObject mSteerObj;
	gkGameObject* gobj;
	//	gkVector3 dir;
//	gkQuaternion rot;
//	gkSteering m_steerUtility;

};

/*
class gsCrowd
{
private:
	static const int AGENT_MAX_TRAIL = 64;
	static const int MAX_AGENTS = 128;
	struct AgentTrail
	{
		float trail[AGENT_MAX_TRAIL*3];
		int htrail;
	};
	AgentTrail m_trails[MAX_AGENTS];

public:

	gsCrowd(gsScene* pScene) : m_luaEvt(0),m_agentRadius(0.5f),m_agentHeight(0.5f){

		if (pScene)
			m_scene = pScene->getOwner();
		else
			m_scene = gkEngine::getSingleton().getActiveScene();
	};
	~gsCrowd(){
		if (m_luaEvt)
			delete m_luaEvt;
	};

	void setCallback(gsSelf self, gsFunction func) {
		if (m_luaEvt)
			delete m_luaEvt;
		m_luaEvt = new gkLuaEvent(self,func);
	}
	void setCallback(gsFunction func) {
		if (m_luaEvt)
			delete m_luaEvt;
		m_luaEvt = new gkLuaEvent(func);
	}

	void init(int maxAgents, float agentRadius,float agentHeight)
	{
		m_agentHeight = agentHeight;
		m_agentRadius = agentRadius;

		if (m_scene && m_scene->hasNavMesh())
		{
			bool result = m_crowd.init(maxAgents,agentRadius,	m_scene->getNavMesh().get()->m_p);
			if (!result) {
				gkLogger::write("Problem creating crowd!");
				return;
			}
		}
		else
		{
			gkLogger::write("crowd: problems retieving navmesh!");
		}

		dtObstacleAvoidanceParams params;
		// Use mostly default settings, copy from dtCrowd.
		memcpy(&params, m_crowd.getObstacleAvoidanceParams(0), sizeof(dtObstacleAvoidanceParams));

		// Low (11)
		params.velBias = 0.5f;
		params.adaptiveDivs = 5;
		params.adaptiveRings = 2;
		params.adaptiveDepth = 1;
		m_crowd.setObstacleAvoidanceParams(0, &params);

		// Medium (22)
		params.velBias = 0.5f;
		params.adaptiveDivs = 5;
		params.adaptiveRings = 2;
		params.adaptiveDepth = 2;
		m_crowd.setObstacleAvoidanceParams(1, &params);

		// Good (45)
		params.velBias = 0.5f;
		params.adaptiveDivs = 7;
		params.adaptiveRings = 2;
		params.adaptiveDepth = 3;
		m_crowd.setObstacleAvoidanceParams(2, &params);

		// High (66)
		params.velBias = 0.5f;
		params.adaptiveDivs = 7;
		params.adaptiveRings = 3;
		params.adaptiveDepth = 3;

		m_crowd.setObstacleAvoidanceParams(3, &params);

	}

	void draw()
	{
		for (int i = 0; i < m_crowd.getAgentCount(); i++)
		{
			const dtCrowdAgent* ag =m_crowd.getAgent(i);
			if (!ag->active)
				continue;

			const dtPolyRef* path = ag->corridor.getPath();
			const int npath = ag->corridor.getPathCount();
			for (int j = 0; j < npath; ++j)
				duDebugDrawNavMeshPoly(&dd, *m_scene->getNavMesh().get()->m_p, path[j], duRGBA(255,255,255,24));
		}

		for (int i = 0; i < m_crowd.getAgentCount(); ++i)
		{
			const dtCrowdAgent* ag = m_crowd.getAgent(i);
			if (!ag->active) continue;

			const float radius = ag->params.radius;
			const float* pos = ag->npos;

			unsigned int col = duRGBA(0,0,0,32);

			duDebugDrawCircle(&dd, pos[0], pos[1], pos[2], radius, col, 2.0f);
		}

		for (int i = 0; i < m_crowd.getAgentCount(); ++i)
		{
			const dtCrowdAgent* ag = m_crowd.getAgent(i);
			if (!ag->active) continue;

			const float height = ag->params.height;
			const float radius = ag->params.radius;
			const float* pos = ag->npos;

			unsigned int col = duRGBA(220,220,220,128);
			if (ag->targetState == DT_CROWDAGENT_TARGET_REQUESTING || ag->targetState == DT_CROWDAGENT_TARGET_WAITING_FOR_QUEUE)
				col = duLerpCol(col, duRGBA(128,0,255,128), 32);
			else if (ag->targetState == DT_CROWDAGENT_TARGET_WAITING_FOR_PATH)
				col = duLerpCol(col, duRGBA(128,0,255,128), 128);
			else if (ag->targetState == DT_CROWDAGENT_TARGET_FAILED)
				col = duRGBA(255,32,16,128);
			else if (ag->targetState == DT_CROWDAGENT_TARGET_VELOCITY)
				col = duLerpCol(col, duRGBA(64,255,0,128), 128);

			duDebugDrawCylinder(&dd, pos[0]-radius, pos[1]+radius*0.1f, pos[2]-radius,
								pos[0]+radius, pos[1]+height, pos[2]+radius, col);
		}
	}

	void tick(const float dt)
	{
		dtNavMesh* nav = m_scene->getNavMesh()->m_p;
		if (!nav) return;

		TimeVal startTime = getPerfTime();

		m_crowd.update(dt, &m_agentDebug);

		TimeVal endTime = getPerfTime();

		// Update agent trails
		for (int i = 0; i < m_crowd.getAgentCount(); ++i)
		{
			const dtCrowdAgent* ag = m_crowd.getAgent(i);
			AgentTrail* trail = &m_trails[i];
			if (!ag->active)
				continue;
			// Update agent movement trail.
			gkVector3 pos(ag->npos);
			gkVector3 vel(ag->nvel);
			gkVector3 vel2(ag->vel);

			if (m_luaEvt)
			{
				m_luaEvt->beginCall();

				m_luaEvt->addArgument(i);
				m_luaEvt->addArgument("gsVector3 *",new gsVector3(pos),1);
				m_luaEvt->addArgument("gsVector3 *",new gsVector3(vel),1);
				m_luaEvt->addArgument("gsVector3 *",new gsVector3(vel2),1);
				m_luaEvt->call();
			}
			else {
				gkLogger::write("c++: idx:"+gkToString(i)+" pos:"+gkToString(pos)+" vel:"+gkToString(vel)+" vel2:"+gkToString(vel2));
			}
			trail->htrail = (trail->htrail + 1) % AGENT_MAX_TRAIL;
			dtVcopy(&trail->trail[trail->htrail*3], ag->npos);
		}

		//m_agentDebug.vod->normalizeSamples();

//		m_crowdSampleCount.addSample((float)m_crowd.getVelocitySampleCount());
//		m_crowdTotalTime.addSample(getPerfDeltaTimeUsec(startTime, endTime) / 1000.0f);

	}

	int addAgent(const gsVector3& pos,float maxAccel=8.0f,float maxSpeed=3.5f)
	{
		gkVector3 startPos(pos);
		std::swap(startPos.y, startPos.z);

		const float* p = startPos.ptr();

		dtCrowdAgentParams ap;
		memset(&ap, 0, sizeof(ap));
		ap.radius = m_agentRadius;
		ap.height = m_agentHeight;
		ap.maxAcceleration = maxAccel;
		ap.maxSpeed = maxSpeed;
		ap.collisionQueryRange = ap.radius * 12.0f;
		ap.pathOptimizationRange = ap.radius * 30.0f;
		ap.updateFlags = 0;
//		if (m_toolParams.m_anticipateTurns)
			ap.updateFlags |= DT_CROWD_ANTICIPATE_TURNS;
//		if (m_toolParams.m_optimizeVis)
			ap.updateFlags |= DT_CROWD_OPTIMIZE_VIS;
//		if (m_toolParams.m_optimizeTopo)
//			ap.updateFlags |= DT_CROWD_OPTIMIZE_TOPO;
//		if (m_toolParams.m_obstacleAvoidance)
//			ap.updateFlags |= DT_CROWD_OBSTACLE_AVOIDANCE;
//		if (m_toolParams.m_separation)
//			ap.updateFlags |= DT_CROWD_SEPARATION;
		ap.obstacleAvoidanceType = (unsigned char)3.0f;
		ap.separationWeight = 2.0f;
		ap.queryFilterType = 0;
		int idx = m_crowd.addAgent(p, &ap);
		if (idx != -1)
		{
			// Init trail
			AgentTrail* trail = &m_trails[idx];
			for (int i = 0; i < AGENT_MAX_TRAIL; ++i)
				dtVcopy(&trail->trail[i*3], p);
			trail->htrail = 0;
		}
		return idx;
	}

	void removeAgent(const int idx)
	{
		m_crowd.removeAgent(idx);
	}


	void requestMove(int idx,const gsVector3& target, const gsVector3 ext = gsVector3(2,4,2))
	{
		gkVector3 tempTargetPos(target);
		std::swap(tempTargetPos.y, tempTargetPos.z);

		const float* p = tempTargetPos.ptr();

		dtPolyRef targetRef;
		float targetPos[3];
		const dtQueryFilter* filter = m_crowd.getFilter(0);

		m_scene->getNavMesh()->m_query->findNearestPoly(target.ptr(), ext.ptr(), filter, &targetRef, targetPos);
		m_crowd.requestMoveTarget(idx, targetRef, targetPos);
	}

private:
	gkLuaEvent* m_luaEvt;

	static void calcVel(float* vel, const float* pos, const float* tgt, const float speed)
	{
		dtVsub(vel, tgt, pos);
		vel[1] = 0.0;
		dtVnormalize(vel);
		dtVscale(vel, vel, speed);
	}

	float m_agentRadius;
	float m_agentHeight;

	dtCrowd m_crowd;
	gkScene* m_scene;
	gkRecastDebugger dd;
	dtCrowdAgentDebugInfo m_agentDebug;
//	ValueHistory m_crowdTotalTime;
//	ValueHistory m_crowdSampleCount;
};
*/

//class gsCrowd
//{
//public:
//	gsCrowd(gsScene* scene) : m_luaEvt(0),m_crowdAvailableDt(0) {
//		if (scene)
//			m_scene = scene->getOwner();
//		else
//			m_scene = gkEngine::getSingleton().getActiveScene();
//
//
//	}
//
//	void init(int numAgents,float radius,float height)
//	{
//		if (m_scene->hasNavMesh())
//		{
//			m_crowd.init(numAgents,radius,m_scene->getNavMesh().get()->m_p);
//		}
//		else
//		{
//			gkLogger::write("Crowd-Init-ERROR:No navmesh");
//		}
//	}
//
//	void setCallback(gsSelf self, gsFunction func) {
//		if (m_luaEvt)
//			delete m_luaEvt;
//		m_luaEvt = new gkLuaEvent(self,func);
//	}
//	void setCallback(gsFunction func) {
//		if (m_luaEvt)
//			delete m_luaEvt;
//		m_luaEvt = new gkLuaEvent(func);
//	}
//
//	void parseCrowd(const gkString jsonInput)
//	{
//		m_sceneCreator.createFromBuffer(jsonInput.c_str());
//		m_sceneCreator.m_currentSample.parseAgentsInfo();
//		m_sceneCreator.m_currentSample.parseCrowd(&m_crowd);
//		m_sceneCreator.m_currentSample.initializeCrowd(&m_crowd);
//	}
//
//	void initalizeCrowd()
//	{
//		m_sceneCreator.m_currentSample.initializeCrowd(&m_crowd);
//	}
//
//	int addAgentType(int idx)
//	{
//		return m_sceneCreator.m_currentSample.addAgent(&m_crowd,idx);
//	}
//
//	void check()
//	{
//		for (int i=0;i<m_crowd.getAgentCount();i++)
//		{
//			const dtCrowdAgent* agent = m_crowd.getAgent(i);
//			if (agent->active)
//			{
//				gkVector3 pos(agent->position);
//				std::swap(pos.y, pos.z);
//				gkVector3 vel(agent->velocity);
//				std::swap(vel.y, vel.z);
//
//				if (m_luaEvt)
//				{
//					m_luaEvt->beginCall();
//
//					m_luaEvt->addArgument(i);
//					m_luaEvt->addArgument("gsVector3 *",new gsVector3(pos),1);
//					m_luaEvt->addArgument("gsVector3 *",new gsVector3(vel),1);
//					m_luaEvt->call();
//				}
//				else {
//					gkLogger::write("c++: idx:"+gkToString(i)+" pos:"+gkToString(pos)+" vel:"+gkToString(vel));
//				}
//			}
//		}
//	}
//
//	void setPosition(unsigned int idx,const gsVector3& pos)
//	{
//		dtCrowdAgent* ag = 0;
//
//		if (!m_crowd.getActiveAgent(&ag, idx))
//			return;
//
//		if (ag)
//		{
//			dtVset(ag->position,pos.x,pos.z,pos.y);
//		}
//	}
//
//	void update(float dt,float maxDt=-1.0f)
//	{
//		bool singleSimulationStep = maxDt == -1.0f;
//
//	        if (singleSimulationStep)
//	        {
//				m_crowd.update(dt);
//
//	            m_crowdAvailableDt = 0.f;
//	        }
//	        else
//	        {
//	        	// TODO: Have a look into that
//	            m_crowdAvailableDt += dt;
//	            while(m_crowdAvailableDt > maxDt)
//	            {
//					m_crowd.update(dt);
//
//	                m_crowdAvailableDt -= dt;
//	            }
//	        }
//
//
//	}
//private:
//	gkLuaEvent* m_luaEvt;
//	gkScene* m_scene;
//	dtCrowd m_crowd;
//	dtSceneCreator m_sceneCreator;
//	float m_crowdAvailableDt;
//};

//class gsAgent
//{
//public:
//	gsAgent(const gsVector3& pos)
//	{
//		m_cfg.position=pos.ptr();
//	}
//
//	void setDestination(const gsVector3& dest)
//	{
//		m_cfg.destination=dest.ptr();
//	}
//
//	void setMaxSpeed(float f)
//	{
//		m_cfg.parameters.maxSpeed=f;
//	}
//	void setMaxAccel(float f)
//	{
//		m_cfg.parameters.maxAcceleration=f;
//	}
//	void setRadius(float f)
//	{
//		m_cfg.parameters.radius=f;
//	}
//	void setHeight(float f)
//	{
//		m_cfg.parameters.height=f;
//	}
//	void setColQueryRange(float f)
//	{
//		m_cfg.parameters.collisionQueryRange=f;
//	}
//	void setPathOptRange(float f)
//	{
//		m_cfg.parameters.pathOptimizationRange=f;
//	}
//	void setSeparationWeight(float f)
//	{
//		m_cfg.parameters.separationWeight=f;
//	}
//private:
//	struct AgentConfiguration
//	{
//	    int index;
//	    float position[3];
//	    float destination[3];
//	    dtPolyRef destinationPoly;
//	    dtCrowdAgentParams parameters;
//	};
//
//	AgentConfiguration m_cfg;
//};


#endif

#endif//_gsAI_h_
