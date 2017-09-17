/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2010 Charlie C.

    Contributor(s): Jonathan.
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
#ifndef _gsCore_h_
#define _gsCore_h_

#include "gsCommon.h"
#include "gsMath.h"
#include "gsUtils.h"
#include "gkMesh.h"
#include "gsPhysics.h"
#include "gkSerialize.h"
#include "OISMultiTouch.h"
#include "utSingleton.h"
#include "Thread/gkSyncObj.h"
#include "CppTweener.h"
#include "Animation/gkAnimation.h"
#include "gsAnimation.h"
#include "gkSettings.h"

#include "OgreStaticGeometry.h"

#include "OgrePrerequisites.h"
#ifdef OGREKIT_COMPILE_LIBCURL
# include "gkDownloadManager.h"
#endif

#ifdef OGREKIT_COMPILE_RECAST
# include "DetourDebugDraw.h"
#endif

//#ifdef OGREKIT_BUILD_OGREPROCEDURAL
//# include "Procedural.h"
//# include "ProceduralUtils.h"
//#endif

#ifdef OGREKIT_COMPILE_OPENSTEER
# include "gkSteeringObject.h"
#endif

#ifdef OGREKIT_COMPILE_LIBROCKET
# include "Rocket/Controls.h"
# include "GUI/rocket/gkGUIDocumentRocket.h"
# include "GUI/gkGUIManager.h"
# include <Rocket/Core/Element.h>
#endif

#ifdef OGREKIT_USE_PROCESSMANAGER
# include "gsProcess.h"
#endif


class gsNavPath;

enum TweenTransition {
	TWT_LINEAR,
	TWT_SINE,
	TWT_QUINT,
	TWT_QUART,
	TWT_QUAD,
	TWT_EXPO,
	TWT_ELASTIC,
	TWT_CUBIC,
	TWT_CIRC,
	TWT_BOUNCE,
	TWT_BACK

};

enum TweenEq {
	TWE_EASE_IN,
	TWE_EASE_OUT,
	TWE_EASE_IN_OUT
};

/// \file
enum gsRenderSystem
{
	GS_RS_GL,
	GS_RS_GLES,
	GS_RS_D3D9,
	GS_RS_D3D10,
	GS_RS_D3D11,
};

enum gsShaderProgrammType {
	ST_VERTEX,
	ST_FRAGMENT
};

enum gsPropertyType
{
	PROP_NULL,
	PROP_BOOL,
	PROP_NUMBER,
	PROP_STRING
};



enum gsTransformSpace
{
	TS_LOCAL,
	TS_PARENT,
	TS_WORLD,
};



enum gsGameObjectTypes
{
	OBT_UNKNOWN = 0,
	OBT_CAMERA,
	OBT_LIGHT,
	OBT_ENTITY,
	OBT_EMPTY,
	OBT_SKELETON,
	OBT_CURVE
};

enum gsLightType {
	LI_POINT,
	LI_SPOT,
	LI_DIR
};

enum gsEngineEvents
{
	EVT_TICK,
};



enum gsMouseButton
{
	LEFT = 0,
	RIGHT,
	MIDDLE,
};


enum gsTouchType
{
	MT_None = 0, MT_Pressed, MT_Released, MT_Moved, MT_Cancelled
};


enum gsCompositorOp 
{ 
	COP_ADD = 0, 
	COP_DEL, 
	COP_REPLACE, 
	COP_RESET 
};

enum gsSceneUpdateFlags
{
UPDATE_NONE			= 0,
UPDATE_PHYSICS		= 1 << 0,
UPDATE_LOGIC_BRICKS	= 1 << 1,
UPDATE_NODE_TREES	= 1 << 2,
UPDATE_ANIMATIONS	= 1 << 3,
UPDATE_SOUNDS		= 1 << 4,
UPDATE_DBVT			= 1 << 5,
UPDATE_DEBUG		= 1 << 6,
UPDATE_ALL			= 0xFFFFFFFF
};

enum gsCompressionLevel {
	COMPRESSION_BEST = 9,
	COMPRESSION_FAST = 1,
	COMPRESSION_NO = 0
};

class gsProperty
{
private:
	bool m_creator;

public:
	gsProperty();
	gsProperty(const gkString& name, bool value);
	gsProperty(const gkString& name, double value);
	gsProperty(const gkString& name, const gkString& value);
	gsProperty(const gsProperty& oth);
	~gsProperty();

	const gkString& getName(void) const;
	const gkString& getValue(void) const;
	void makeDebug(bool v);
	gsPropertyType getType(void) const;
	bool toBool(void) const;
	double  toNumber(void) const;
	gkString toString(void) const;
	gsVector3 toVec3(void) const;
	void fromBool(bool v);
	void  fromNumber(double v);
	void fromString(const gkString& v);
	void fromVector3(gsVector3 v);

	// internal wrap
	OGRE_KIT_WRAP_CLASS_COPY_CTOR(gsProperty, gkVariable, m_prop);
};


class gsUserDefs
{
private:
	typedef utHashTable<gkHashedString, gsProperty*> PropertyMap;
	PropertyMap m_properties;

	void setValueEvent(gkVariable& v);
public:
	~gsUserDefs();

	const gsProperty& getProperty(const gkString& name);
	void addProperty(const gsProperty& prop);
	bool hasProperty(const gkString& name);

	const gsProperty&    __getitem__(const char* name);
	void                __setitem__(const char* name, bool  v);
	void                __setitem__(const char* name, double v);
	void                __setitem__(const char* name, const gkString& v);

	// internal wrap
	OGRE_KIT_WRAP_CLASS_COPY_CTOR(gsUserDefs, gkUserDefs, m_defs);
};


typedef OIS::MultiTouchState gkMultiTouchState;

class gsMultiTouchState
{
public:
	gsMultiTouchState(OIS::MultiTouchState* state)
		: m_state(state)
	{}

	int getPointerId() {
		if (!m_state)
			return 0;
		return m_state->fingerID;
	}

	int getType() {
		if (!m_state)
			return 0;
		return m_state->touchType;
	}

	int getX() {
		if (!m_state)
			return 0;
		return m_state->X.abs;
	}

	int getY() {
		if (!m_state)
			return 0;
		return m_state->Y.abs;
	}

	float getPressure() {
		if (!m_state)
			return 0;
		return m_state->pressure;
	}

	float getSize() {
		if (!m_state)
			return 0;
		return m_state->size;
	}


private:
	OIS::MultiTouchState* m_state;
};



class gsTouch
{
public:
	gsTouch();
	~gsTouch();

	class gsTouchListener : public OIS::MultiTouchListener
	{
	public:
		gsTouchListener(gsSelf self,gsFunction func) : m_event(self,func){}
		gsTouchListener(gsFunction func) : m_event(func){}
		virtual ~gsTouchListener(){}

		bool touchMoved( const OIS::MultiTouchEvent &arg )
		{
			m_event.beginCall();
			m_event.addArgument(gkString("moved"));
			setDefaultArguments(m_event,arg);
			m_event.call();
			return false;
		}

			bool touchPressed( const OIS::MultiTouchEvent &arg )
			{
				m_event.beginCall();
				m_event.addArgument(gkString("pressed"));
				setDefaultArguments(m_event,arg);
				m_event.call();
				return false;

			}
			bool touchReleased( const OIS::MultiTouchEvent &arg )
			{
				m_event.beginCall();
				m_event.addArgument(gkString("released"));
				setDefaultArguments(m_event,arg);
				m_event.call();
				return false;
			}
			bool touchCancelled( const OIS::MultiTouchEvent &arg )
			{
				m_event.beginCall();
				m_event.addArgument(gkString("canceled"));
				setDefaultArguments(m_event,arg);
				m_event.call();
				return false;
			}
	private:
			gkLuaEvent m_event;

			void setDefaultArguments(gkLuaEvent& evt, const OIS::MultiTouchEvent& arg)
			{
				evt.addArgument((int)arg.state.fingerID);
				evt.addArgument((int)arg.state.X.abs);
				evt.addArgument((int)arg.state.Y.abs);
			}
	};


	void addListener(gsSelf self,gsFunction func);
	void addListener(gsFunction func);

	int getNumStates();
	gkMultiTouchState* getState(int i);

private:
	OIS::MultiTouch* m_touch;
	OIS::MultiTouchState m_nullState;
	typedef utList<gsTouchListener*> TouchListeners;
	TouchListeners m_listeners;
};

class gsMouse
{
public:
	gsMouse();
	~gsMouse();

	float   xpos, ypos;
	float   xrel, yrel;
	float   winx, winy;
	float   wheel;
	bool    moved;

	void capture(void);
	bool isButtonDown(int btn);
};

extern int getNumJoysticks(void);

class gsJoystick
{
	gkJoystick m_joystick;
	int m_index;
	
public:
	gsJoystick(int i);
	~gsJoystick();
	
	void capture(void);
	
	int getNumAxes();
	int getAxis(int i);
	int getRelAxis(int i);

	gsVector3 getAccel();
	
	int getNumButtons();
	int getButtonCount();
	bool isButtonDown(int i);
	bool wasButtonPressed(int i);
	
	int getWinWidth(void);
	int getWinHeight(void);
};


class gsKeyboard
#ifndef SWIG
	: gkWindowSystem::Listener
#endif
{
public:
	gsKeyboard();
	~gsKeyboard();
	void capture(void) {}
	void clearKey(int sc);

	bool isKeyDown(int sc);
	bool isKeyUp(int sc);


	void setCallback(gsSelf self,gsFunction func);
	void setCallback(gsFunction func);
private:
	gkLuaEvent* m_event;

	virtual void keyPressed(const gkKeyboard& key, const gkScanCode& sc) ;
	virtual void keyReleased(const gkKeyboard& key, const gkScanCode& sc) ;



};





class gsEngine
#ifndef SWIG
	: public gkEngine::Listener
#endif
{
private:
    static gsEngine* m_singleton;
	bool m_ctxOwner, m_running;
	gsUserDefs* m_defs;
	typedef utArray<gkLuaEvent*> Connectors;

	Connectors m_ticks;

	void tick(gkScalar rate);

public:

	gsEngine();
	~gsEngine();

	void initialize(void);
	void run(void);
	void requestExit(void);
	void saveTimestampedScreenShot(const gkString& filenamePrefix, const gkString& filenameSuffix);

	// connect global function
	void connect(int evt, gsFunction func);

	// connect self:function
	void connect(int evt, gsSelf self, gsFunction method);

	void loadBlendFile(const gkString& name, const gkString& sceneName="",const gkString& groupName="",bool ignoreLibraries=true);
	gkScene* getActiveScene(void);

	gkScene* getScene(const gkString& sceneName,const gkString& groupName="");
	gkScene* createEmptyScene(const gkString& sceneName,const gkString& camName,const gkString& groupName);

	gkScene* addOverlayScene(gsScene* scene);
	gkScene* addOverlayScene(const gkString& name);

	gkScene* addBackgroundScene(gsScene* scene);
	gkScene* addBackgroundScene(const gkString& name);

	void removeScene(gsScene* scene);
	void removeScene(const gkString& name);

	//utSingleton &operator = (const gsEngine& rhs) {return *this;}


#ifndef SWIG
	gkScene* addOverlayScene(gkScene* scene);
	gkScene* addBackgroundScene(gkScene* scene);
	void removeScene(gkScene* scene);

	//UT_DECLARE_SINGLETON(gsEngine);
	static gsEngine& getSingleton(void);                     \
	static gsEngine* getSingletonPtr(void);
#endif

	void setSystemMode(bool b) { m_isSystemEngine = b;}
	bool isSystemMode() { return m_isSystemEngine; }

	gsUserDefs& getUserDefs(void);




	// internal wrap
	OGRE_KIT_WRAP_CLASS(gkEngine, m_engine);


private:
	bool m_isSystemEngine; // if yes it will not be stopped on debug singlestep-mode
};




class gsObject
{
public:
	gsObject();

	void createInstance(void);
	void destroyInstance(void);
	void reinstance(void);
	bool isInstanced() { return m_object->isInstanced(); }
	gkString getName(void);
	gkString getGroupName(void);

	// internal wrap
	OGRE_KIT_WRAP_CLASS_COPY_CTOR(gsObject, gkInstancedObject, m_object);
	OGRE_KIT_INTERNAL_CAST(m_object);
};

class gsHUD
{
public:
	gsHUD();

	void show(bool v);

	gkHUDElement* getChild(const gkString& child);

	// internal wrap
	OGRE_KIT_WRAP_CLASS_COPY_CTOR(gsHUD, gkHUD, m_object);
	OGRE_KIT_INTERNAL_CAST(m_object);
	//OGRE_KIT_TEMPLATE_NEW_INLINE(gsHUD, gkHUD);
};

class gsHUDElement
{
public:
	gsHUDElement();

	void show(bool v);

	gkString getValue();
	void setValue(const gkString& value);

	void setUvCoords(float u1, float v1, float u2, float v2);
	
	gkString getParameter(const gkString& name);
	void setParameter(const gkString& name, const gkString& value);

	gkString getMaterialName();
	void setMaterialName(const gkString& material);

	float getMaterialAlpha();
	void setMaterialAlpha(float factor);

	int getMaterialAlphaRejectValue();
	void setMaterialAlphaRejectValue(int val, bool isGreater);
	gsVector3 getPosition();
	void setPosition(gsVector3 pos);
	void setPosition(float x,float y);
	// internal wrap
	OGRE_KIT_WRAP_CLASS_COPY_CTOR(gsHUDElement, gkHUDElement, m_object);
	OGRE_KIT_INTERNAL_CAST(m_object);
	//OGRE_KIT_TEMPLATE_NEW_INLINE(gsHUDElement, gkHUDElement);

private:
	gsVector3 mPosition;
};

extern gkHUD* getHUD(const gkString& name);

class gsStaticBatch
{
public:
	gsStaticBatch(gkScene* scene) : mBatch(0),mScene(scene){ if (!scene) mScene=gkEngine::getSingleton().getActiveScene();}
	~gsStaticBatch();

	void addEntity(gsEntity* ent);
	void removeEntity(gsEntity* ent);

	void build();
private:
	void _addEntityWithChildren(gkGameObject* ent);
	Ogre::StaticGeometry* mBatch;
	typedef utList<gsEntity*> StaticEntityList;
	StaticEntityList mStaticEntityList;
	gkScene* mScene;
};

namespace Ogre
{
	class HlmsManager;
	class PbsMaterial;
}


class gsScene : public gsObject
{
private:
	gsArray<gsGameObject, gkGameObject> m_objectCache;
	gsRay* m_pickRay;
#ifdef OGREKIT_USE_PROCESSMANAGER
	gkProcessManager* m_processManager;
#endif

public:
	gsScene();
	~gsScene();


	bool hasObject(const gkString& name);

	gkGameObject* getObject(const gkString& name);
	gkGameObject* createEmpty(const gkString& name);
	gkGameObject* createEntity(const gkString& name);
	gkGameObject* createCamera(const gkString& name);
	gkLight* createLight(const gkString&, gsLightType lightType);
    gkGameObject* cloneObject(gsGameObject* obj, int lifeSpan, bool instantiate);
    void destroyObject(gsGameObject* gobj);

	gsArray<gsGameObject, gkGameObject> &getObjectList(void);

	gkDynamicsWorld* getDynamicsWorld(void);

	gkCamera* getMainCamera(void);

	gsRay* getPickRay(float x=-1,float y=-1);

	void setUpdateFlags(unsigned int flags);

	void setLayer(int layer);
	int getLayer(void);

	void setBackgroundColor(gsVector4 col);

	gkRecastDebugger* getRecastDebugger();
	gkDebugger* getDebugger();

	gsVector2 getScreenPos(const gsVector3& vec,bool absolute=true,bool outerFrustum=false);

	gsStaticBatch* createStaticBatch() {
		gsStaticBatch* newBatch = new gsStaticBatch(getOwner());
		mStaticBatches.push_back(newBatch);
		return newBatch;
	}

#ifdef OGREKIT_USE_PROCESSMANAGER
	gkProcessManager* getProcessManager(void);
#endif

	gkLogicManager* getLogicBrickManager(void);

	gkScene* getOwner();

	void asyncCreateNavMesh(float agentRadius=1.0f, float agentHeight=1.0f,float agentMaxClimb=0.1f) {
		gkRecast::Config config;
		config.AGENT_HEIGHT = agentHeight;
		config.AGENT_RADIUS = agentRadius;
		config.AGENT_MAX_CLIMB = agentMaxClimb;

		getOwner()->createNavMesh(config);
	}

	bool hasNavMesh() {
		return getOwner()->hasNavMesh();
	}

	void setNavMeshCallback(gsSelf self,gsFunction func)
	{
		getOwner()->setNavMeshCallback(self,func);
	}

	void setNavMeshCallback(gsFunction func)
	{
		getOwner()->setNavMeshCallback(func);
	}

	gsVector3 getNextPointOnNavmesh(const gsVector3& vec,const gsVector3 polyPickExt=gsVector3(2,4,2))
	{
		return getOwner()->getNextPointOnNavmesh(gkVector3(vec),polyPickExt);
	}

	void setPBS(float f0,float r,float fr,const gkString& entName,const gkString& albedo="",const gkString& normalmap="",const gkString& envmap="");
	void createHLMSMaterial(Ogre::Entity *ent, Ogre::PbsMaterial* pbsMaterial, const gkString& matName);

	// internal
	OGRE_KIT_WRAP_BASE_COPY_CTOR(gsScene, gkInstancedObject);
private:
	typedef utList<gsStaticBatch*> StaticBatches;
	StaticBatches mStaticBatches;
	Ogre::HlmsManager* m_hlmsManager;
	Ogre::vector<Ogre::PbsMaterial*>::type mPBSMaterialList;
	Ogre::vector<gkString>::type mMaterialList;
};


extern gkScene* getActiveScene(void);
extern gkScene* getScene(const gkString& sceneName,const gkString& groupName="");
extern gkScene* addScene(const gkString& sceneName,int zorder);
extern gkScene* addScene(gsScene* scene,int zorder);
class gsGameObjectInstance
{
public:
	gsGameObjectInstance( gkGameObjectInstance* inst );
	virtual ~gsGameObjectInstance();

	int getElementCount();
	gkGameObject* getElementAt(int pos);
	gkGameObject* getElementByName(gkString name);
	gkGameObject* getRoot(void);

	bool isInstanced();
	void destroyInstance();
	void kill();
	void createInstance();
	void reinstance();
	gkString getName();
	gkString getGroupName();
private :
	gkGameObjectInstance* m_gobj;
};

/// \brief createGroupInstance
extern gkGameObjectInstance* createGroupInstance(const gkString& name,gsVector3 loc=gsVector3(0,0,0),gsVector3 orientation=gsVector3(0,0,0),gsVector3 scale=gsVector3(1,1,1),const gkString& resGroup="");
extern gkGameObjectInstance* createGroupInstance(gsScene* scene,const gkString& name,gsVector3 loc=gsVector3(0,0,0),gsVector3 orientation=gsVector3(0,0,0),gsVector3 scale=gsVector3(1,1,1),const gkString& resGroup="");
extern gkGameObjectInstance* createGroupInstance(gkScene* scene,const gkString& name,gsVector3 loc=gsVector3(0,0,0),gsVector3 orientation=gsVector3(0,0,0),gsVector3 scale=gsVector3(1,1,1),const gkString& resGroup="");
extern gkGameObjectInstance* createGroupInstance(gsScene* scene,const gkString& name,const gkString& resGroup);



extern void updateStaticBatch(gkString groupName="",gsScene* scene=0);
extern bool groupExists(gkString name);
extern int getGroupCount(gsScene* gsscene=0);
extern gkString getGroupNameAt(int i,gsScene* gsscene=0);

extern void destroyResourceGroup(const gkString& resGrpName);

class gsContactInfo
{
public:
	gsContactInfo()
		: appliedImpulse(0),appliedImpulseLateral1(0),appliedImpulseLateral2(0)
		 ,contactMotion1(0),contactMotion2(0),contactCFM1(0),contactCFM2(0),lifeTime(0)
		{}
	gsContactInfo(const btManifoldPoint& pnt)
		: appliedImpulse(pnt.m_appliedImpulse),appliedImpulseLateral1(pnt.m_appliedImpulseLateral1),appliedImpulseLateral2(pnt.m_appliedImpulseLateral2)
		 ,contactMotion1(pnt.m_contactMotion1),contactMotion2(pnt.m_contactMotion2),contactCFM1(pnt.m_contactCFM1),contactCFM2(pnt.m_contactCFM2),lifeTime(pnt.m_lifeTime)
	{}
	float appliedImpulse;
	float appliedImpulseLateral1;
	float appliedImpulseLateral2;
	float contactMotion1;
	float contactMotion2;
	float contactCFM1;
	float contactCFM2;
	float lifeTime;
};

class gsGameObject : public gsObject
{
public:
	gsGameObject();
	~gsGameObject() {
		if (m_callback)
			delete m_callback;
	}
	gkString  getOriginalName(void) { return get()->getOriginalName(); }
	gsVector3 getPosition(void);
	gsVector3 getRotation(void);
	gsQuaternion getOrientation(void);
	gsVector3 getScale(void);
	gsVector3 getWorldPosition(void);
	gsVector3 getWorldRotation(void);
	gsVector3 getWorldScale(void);
	void setTransform(const gsMatrix4& vmat);

	void bakeTransform();

	int getFilterMask() { return get()->getProperties().m_physics.m_colMask << 5; }
	int getFilterGroup() { return get()->getProperties().m_physics.m_colGroupMask << 5; }

	gsVector2 getScreenPos(bool absolute=true);

	gsQuaternion getWorldOrientation(void);
	gsVector3 getLinearVelocity(void);
	gsVector3 getAngularVelocity(void);

	gsVector3 getLinearFactor(void);
	void 	  setLinearFactor(const gsVector3& v);
	gsVector3 getAngularFactor(void);
	void      setAngularFactor(const gsVector3& v);

	void	  setGravity(const gsVector3& v);
	gsVector3 getGravity(void);

	void clearForces(void);
	void setMass(gkScalar mass);

	void applyForce(const gsVector3& v, gsTransformSpace ts=TS_PARENT);
	void applyForce(float x, float y, float z, gsTransformSpace ts=TS_PARENT);


	void setLinearVelocity(const gsVector3& v, gsTransformSpace ts=TS_PARENT);
	void setLinearVelocity(float x, float y, float z, gsTransformSpace ts=TS_PARENT);

	void setAngularVelocity(const gsVector3& v, gsTransformSpace ts=TS_PARENT);
	void setAngularVelocity(float x, float y, float z, gsTransformSpace ts=TS_PARENT);

	void suspendPhysics(bool suspend);

	void setPosition(const gsVector3& v);
	void setPosition(float x, float y, float z);

	void setRotation(const gsVector3& v);
	void setRotation(float pitch, float yaw, float roll);

	void lookAt(const gsVector3& v);
	void lookAt(gsGameObject* lookAtObj);
	gsQuaternion fakeLookAt(const gsVector3& v);
	
	void setOrientation(const gsQuaternion& quat);
	void setOrientation(float w, float x, float y, float z);

	void setScale(const gsVector3& v);
	void setScale(float x,float y,float z);

	gsGameObjectTypes getType(void);

	void rotate(float dx, float dy, float dz);
	void rotate(const gsVector3& v);
	void rotate(const gsQuaternion& v);
	void rotate(float dx, float dy, float dz, gsTransformSpace ts);
	void rotate(const gsVector3& v, gsTransformSpace ts);
	void rotate(const gsQuaternion& v, gsTransformSpace ts);

	void tweenToPos(const gsVector3& to,float ptime,TweenTransition ptransition = TWT_EXPO, TweenEq pequation = TWE_EASE_OUT, float pdelay = 0);

	void translate(float x, float y, float z);
	void translate(const gsVector3& v);
	void translate(float x, float y, float z, gsTransformSpace ts);
	void translate(const gsVector3& v, gsTransformSpace ts);

	void scale(const gsVector3& v);
	void scale(float x, float y, float z);
	gsVector3 getDimension();

	void yaw(float deg);
	void yaw(float deg, gsTransformSpace ts);


	void pitch(float deg);
	void pitch(float deg, gsTransformSpace ts);


	void roll(float deg);
	void roll(float deg, gsTransformSpace ts);

	int getState(void);
	void changeState(int v);

	bool hasParent();
	void setParent(gsGameObject* par);
	void setParentInPlace(gsGameObject* par);
	void clearParent(void);
	void clearParentInPlace(void);
	void addChild(gsGameObject* chi);
	void removeChild(gsGameObject* chi);
	gkGameObject* getChildAt(int pos);
	int getChildCount(void);
	gkGameObject* getChildByName(const gkString& name);


	gkGameObject* getParent(void);

	void enableContacts(bool v);
	bool hasContacts();
	bool hasContact(const gkString& object);
	int  getContactCount();
	gkGameObject* getContact(int nr);
	gsContactInfo getContactInfo(int nr);

	bool isVehicle();
	gkVehicle* getVehicle();

	gkLogicTree* getLogicTree();
//	void explicitContactTest(gsFunction func);

	gkScene* getScene(void);
	
	gkAnimationPlayer* getAnimation(const gkString& name, bool loop=true);
	void playAnimation(gsAnimationPlayer* anim,float blend=0, bool restart=false);
	gkAnimationPlayer* playAnimation(const gkString& name, float blend, bool restart=false, bool loop=true);
	void stopAnimation(const gkString& name);
	void pauseAnimations();
	void resumeAnimations();
	int getAnimationCount();
	gkString getAnimationName(int nr);

	void setVisible(bool b);
	bool isVisible();
	// actually just calling not lua-enabled method!
	gsProperty getProperty(const gkString& name) {return __getitem__(name);}
	bool       hasProperty(const gkString& name);
	gkVariable* getPropertyRaw(const gkString& name);
	void        setProperty(const gkString& prop, gsVector3  vec) {__setitem__(prop,vec);}
	void        setProperty(const gkString& prop, bool  v) {__setitem__(prop,v);}
	void       setProperty(const gkString& prop, float v){__setitem__(prop,v);}
	void        setProperty(const gkString& prop, gkString v){__setitem__(prop,v.c_str());}

	gsProperty  __getitem__(const gkString& prop);
	void        __setitem__(const gkString& prop, bool  v);
	void        __setitem__(const gkString& prop, float v);
	void        __setitem__(const gkString& prop, const char* v);
	void        __setitem__(const gkString& prop, gsVector3 v);

	void addCallback(gsSelf self,gsFunction func );
	void addCallback(gsFunction func);
	void removeCallback();


	gkGameObjectInstance* getGroupInstance();
	bool             isGroupInstance();

	// internal
	OGRE_KIT_WRAP_BASE_COPY_CTOR(gsGameObject, gkInstancedObject);
	OGRE_KIT_TEMPLATE_CAST(gkGameObject, m_object);
//	OGRE_KIT_TEMPLATE_NEW(gsGameObject, gkGameObject);
private:

	bool isinfrustum(const gkVector3& coords);
	bool isinfront(const gkVector3& worldview);

	class gsGameObjectCallback : public gkGameObject::Notifier {
	public:
		gsGameObjectCallback(gkGameObject* gobj,gkLuaEvent* event) : m_event(event)
		{}
		~gsGameObjectCallback() {
			delete m_event;
		}

		void notifyGameObjectEvent(gkGameObject* gobj, const Event& id) {
			m_event->beginCall();
			m_event->addArgument((int)id);
			//m_event->addArgument(gobj->getName());
			bool b=false;
			m_event->call(b);
		}

	private:
		gkLuaEvent* m_event;
	};


	gsGameObjectCallback* m_callback;
};

class gsMessageListenerCallback
#ifndef SWIG
		: public gkMessageManager::GenericMessageListener
#endif
{
public:
	// class-method-callback
	gsMessageListenerCallback(const gkString& from,const gkString& to, const gkString& topic,gsSelf self,gsFunction func)
		: gkMessageManager::GenericMessageListener(from,to,topic), m_event(self,func)
	{
	}

	// function-callback
	gsMessageListenerCallback(const gkString& from,const gkString& to, const gkString& topic,gsFunction func)
		: gkMessageManager::GenericMessageListener(from,to,topic), m_event(func)
	{
	}

	~gsMessageListenerCallback()
	{
		gkMessageManager::getSingleton().removeListener(this);
		gkLogger::write("destructor: remove lua-message-listener",true);
	}

	void handleMessage(gkMessageManager::Message* message) {
		if (!m_fromFilter.empty() && m_fromFilter.compare(message->m_from) != 0) return;
		int i =  m_toFilter.compare(message->m_to);
		if ((!m_toFilter.empty() && m_toFilter.compare(message->m_to) != 0)) return;
		if (!m_subjectFilter.empty() && m_subjectFilter.compare(message->m_subject) != 0) return;

		m_event.beginCall();
		m_event.addArgument(message->m_from);
		m_event.addArgument(message->m_to);
		m_event.addArgument(message->m_subject);
		m_event.addArgument(message->m_body);
		m_event.call();
	};

private:
	gkLuaEvent m_event;
};

class gsMessageManager
{
public:
	gsMessageManager( ){}

	gsMessageListenerCallback* addListener(const gkString& from,const gkString& to, const gkString& topic,gsSelf self,gsFunction func) {
		gsMessageListenerCallback* cb = new gsMessageListenerCallback(from,to,topic,self,func);
		gkMessageManager::getSingleton().addListener(cb);
		return cb;
	}

	gsMessageListenerCallback* addListener(const gkString& from,const gkString& to, const gkString& topic,gsFunction func) {
		gsMessageListenerCallback* cb = new gsMessageListenerCallback(from,to,topic,func);
		gkMessageManager::getSingleton().addListener(cb);
		return cb;
	}


	void removeListener(gsMessageListenerCallback* callback) {
		if (callback) {
			delete callback;
		}
	}
};

class gsCurve : public gsGameObject
{
public:
	gsCurve();
	~gsCurve() {}
	OGRE_KIT_WRAP_BASE_COPY_CTOR(gsCurve, gkInstancedObject);

	void generateBezierPoints(float nr);
	gsVector3 getPoint(int nr,int subcurve=0);
	int getPointCount(int subcurve=0);
	bool isCyclic(int subcurve=0);
	bool isBezier(int subcurve=0);
	gsVector3 getDeltaPoint(float dt,int subcurve=0);
	int getSubCurveAmount();
};

class gsLight : public gsGameObject
{
public:
	gsLight();
	~gsLight() {}



	float getPowerScale() {
		if (m_object) {
			return ((gkLight*)m_object)->getPowerScale();
		}
		return 0;
	}

	void setPowerScale(float powerScale) {
		if (m_object) {
			((gkLight*)m_object)->setPowerScale(powerScale);
		}
	}
	// internal
	OGRE_KIT_WRAP_BASE_COPY_CTOR(gsLight, gkInstancedObject);
};



class gsCamera : public gsGameObject
{
public:
	gsCamera();
	~gsCamera() {}

	void setClipping(float start, float end);
	float getClipStart();
	float getClipEnd();
	void setProjection(const gsMatrix4& mat4);

	void setFov(float fov);
	float getFov();

	void makeCurrent();

	bool isOrtho();
	void setOrthoScale(float scale);
	float getOrthoScale();

	// internal
	OGRE_KIT_WRAP_BASE_COPY_CTOR(gsCamera, gkInstancedObject);
};

class gsCharacter;

class gsEntity : public gsGameObject
{
public:

	gsEntity();
	~gsEntity() {}

	bool hasCharacter(void);
	gkCharacter* getCharacter(void);
	void setMaterialName(const gkString& materialName);
	void setOgreMeshName(const gkString& meshName);
	void playOgreAnimation(const gkString& animationName,bool loop=true);
	void updateOgreAnimation(float dt);
	gkMesh* getMesh();
	bool hasHardwareSkinning();
	// internal
	OGRE_KIT_WRAP_BASE_COPY_CTOR(gsEntity, gkInstancedObject);
private:
	Ogre::AnimationState* mAnimState;

};



class gsSkeleton : public gsGameObject
{
public:
	gsSkeleton();
	~gsSkeleton() {}

	void attachObjectToBone(const gkString& bone, gsGameObject* gsobj,gsVector3 loc=gsVector3(0,0,0),gsVector3 orientation=gsVector3(0,0,0),gsVector3 scale=gsVector3(1,1,1));
	void attachObjectToBoneInPlace(const gkString& bone, gsGameObject* gsobj);

	void setBoneManual(const gkString& boneName,bool manual);
	bool isBoneManual(const gkString& boneName);

	void applyBoneChannelTransform(const gkString& boneName, gsVector3 loc=gsVector3(0,0,0), gsVector3 orientation=gsVector3(0,0,0),gsVector3 scale=gsVector3(1,1,1), gkScalar pWeight=1.0F);

	gsVector3 getBonePosition(const gkString& boneName);
	// internal
	OGRE_KIT_WRAP_BASE_COPY_CTOR(gsSkeleton, gkInstancedObject);
};

class gsParticles : public gsGameObject
{
public:
	gsParticles();
	~gsParticles() {}



	// internal
	OGRE_KIT_WRAP_BASE_COPY_CTOR(gsParticles, gkInstancedObject);
};



class gsDebugger
{
protected:
	class gkDebugger* m_debugger;

public:
	gsDebugger(gkDebugger* db);
	gsDebugger(gsScene* sc);
	~gsDebugger() {}

	void drawLine(const gsVector3& from, const gsVector3& to, const gsVector3& color=gsVector3(1,1,1));
	void drawObjectAxis(gsGameObject* ptr, float size);
	void drawCurve(gsCurve* path, const gsVector3& color=gsVector3(1,1,1));
	void drawCircle(const gsVector3& center,float radius,const gsVector3& color=gsVector3(1,1,1));
	void drawPath(gsNavPath* path,const gsVector3& color=gsVector3(1,1,1));
	void clear(void);
};

class gsRecastDebugger
{
public:
	gsRecastDebugger(gkRecastDebugger* rd) {
		m_recastDebugger = rd;
		mManagedbyme = false;
	}

	gsRecastDebugger(gsScene* scene,const gsVector3& pos=gsVector3(0,0,0),const gsVector3& rot=gsVector3(90,0,0))
		: m_pos(pos),m_rot(rot)
	{
		mManagedbyme=true;
		m_recastDebugger = new gkRecastDebugger(scene->getOwner());
	}
	~gsRecastDebugger(){
		if (mManagedbyme)
			delete m_recastDebugger;
	}

	void drawCircle(const gsVector3& pos,float radius,float lineWidth=1.0f,const gsVector4& col=gsVector4(1.0,0.25,0.5,1.0))
	{
		Ogre::ColourValue tempCol(col.x,col.y,col.z,col.w);
		m_recastDebugger->setYZConversionMode(true);
		duDebugDrawCircle(m_recastDebugger,pos.x,pos.z,pos.y,radius,(unsigned int)tempCol.getAsRGBA(),lineWidth);
		m_recastDebugger->setYZConversionMode(false);
		m_recastDebugger->flush(m_pos,m_rot);
	}

	void drawCylinder(gsVector3& center,gsVector3& size,const gsVector4& col=gsVector4(1.0,0.25,0.5,1.0))
	{
		Ogre::ColourValue tempCol(col.x,col.y,col.z,col.w);
		gkVector3 half(size / 2);
		gkVector3 bL(center - half);
		gkVector3 tR(center + half);
		duDebugDrawCylinderWire(m_recastDebugger,bL.x,bL.z,bL.y,tR.x,tR.z,tR.y,(unsigned int)tempCol.getAsRGBA(),1.0f);
	}

	void drawBox(const gsVector3& bL,const gsVector3& tR,const gsVector4& col=gsVector4(1.0,0.25,0.5,1.0))
	{
		Ogre::ColourValue tempCol(col.x,col.y,col.z,col.w);

		duDebugDrawBoxWire(m_recastDebugger,bL.x,bL.y,bL.z,tR.x,tR.y,tR.z,tempCol.getAsRGBA(),1);
	}

	void drawCross(const gsVector3& center,float size,const gsVector4& col=gsVector4(1.0,0.25,0.5,1.0))
	{
		Ogre::ColourValue tempCol(col.x,col.y,col.z,col.w);
		duDebugDrawCross(m_recastDebugger,center.x,center.y,center.z,size,tempCol.getAsRGBA(),1);
	}

#ifdef OGREKIT_COMPILE_RECAST
	void drawNavMesh(gsScene* scene)
	{
		gkScene* pscene = scene->getOwner();

		if (!pscene->hasNavMesh())
			return;

		// TODO:
		m_recastDebugger->setYZConversionMode(true);
		gkDetourNavMesh* navmesh = pscene->getNavMesh().get();
		if (navmesh)
		{
			duDebugDrawNavMesh(m_recastDebugger,*navmesh->m_p,1);
			m_recastDebugger->setYZConversionMode(false);
		}
	}
#endif

	void drawLine(const gsVector3& from,const gsVector3& to,const gsVector4& col) {
		Ogre::ColourValue tempCol(col.x,col.y,col.z,col.w);
		m_recastDebugger->drawLine(from,to,tempCol.getAsRGBA());
	}
	void drawTri(const gsVector3& t1,const gsVector3& t2,const gsVector3& t3,const gsVector4& col=gsVector4(1.0,0.75,0.75,1.0)) {
		Ogre::ColourValue tempCol(col.x,col.y,col.z,col.w);
		m_recastDebugger->drawTriangle(t1,t2,t3,tempCol.getAsRGBA());
	}

	void clear() {
		m_recastDebugger->clear();
	}


	void drawPath(gsNavPath* path,const gsVector4& col = gsVector4(1.0f,0.75f,0.75f,1.0f));

private:
	gkRecastDebugger* m_recastDebugger;

	gkVector3 m_pos;
	gkVector3 m_rot;
	bool mManagedbyme;
};

class gsSubMesh {
	friend class gsMesh;
public:
	gsSubMesh();
	gsSubMesh(gkSubMesh* submesh);
	~gsSubMesh();

	gkVertex* getVertex(int nr){ return &(m_submesh->getVertexBuffer().at(nr));};
	int getVertexAmount(){ return m_submesh->getVertexBuffer().size();};
	const gkTriangle addTriangle(const gkVertex& v0,
            unsigned int i0,
            const gkVertex& v1,
            unsigned int i1,
            const gkVertex& v2,
            unsigned int i2);

	gkMaterialProperties& getMaterial(void)                 {return m_submesh->getMaterial();}
	void setUVCount(int count) { m_submesh->setTotalLayers(1);}
	int getUVCount() { return m_submesh->getUvLayerCount();}
	void setMaterialName(const gkString& materialName);

private:
	gkSubMesh* m_submesh;
	bool m_isMeshCreator;
};

class gsMesh
{
public:
	gsMesh(gkMesh* mesh);
	~gsMesh() {};
	int getSubMeshAmount() { return m_mesh->m_submeshes.size();}
	gkSubMesh* getSubMesh(int nr) { return m_mesh->m_submeshes.at(nr); }
	void addSubMesh(gsSubMesh* submesh);
	void reload() {m_mesh->reload();}
private:
	gkMesh* m_mesh;
};

//typedef Procedural::MeshGenerator gkMeshGenerator;
//
//class gsMeshGenerator {
//
//};
class gsTween
#ifndef SWIG
	: tween::TweenCallback
#endif
{
public:

	gsTween(float time,gsSelf self,gsFunction func,const gkString& id="")
		: m_time(time), m_event(self,func), m_keepRunning(true),m_tweenParam(0),m_running(false),m_id(id)
	{
		m_tweenParam.time = time;
		m_tweenParam.callback = this;
	}

	gsTween(float time,gsFunction func,const gkString& id="")
		: m_time(time), m_event(func), m_keepRunning(true),m_tweenParam(0),m_running(false),m_id(id)
	{
		m_tweenParam.time = time;
		m_tweenParam.callback = this;
	}

	~gsTween()
	{
	}


	bool start()
	{
		if (!m_running && m_initValues.size() > 0)
		{
			m_tweenParam.cleanProperties();
			m_tvalues.clear();

			InitValues::Iterator iter(m_initValues);
			while (iter.hasMoreElements())
			{
				std::pair<float,float> iValue = iter.getNext();
				int pos = m_tvalues.size();
				m_tvalues.push_back(iValue.first);
				m_tweenParam.addProperty(&m_tvalues[pos],iValue.second);
			}

			m_running = true;
			gkTweenManager::getSingleton().addTween(m_tweenParam);
			return true;
		}
		return false;
	}

	void addValue(float from,float to)
	{
		m_initValues.push_back(std::pair<float,float>(from,to));
	}

	bool onTweenStep()
	{
		m_event.beginCall();

		if (!m_id.empty())
			m_event.addArgument(m_id);

		m_event.addArgument(gkString("step"));
		m_event.addArgument(m_tweenParam.timeCount);
		TweenValues::Iterator iter(m_tvalues);
		while(iter.hasMoreElements())
		{
			m_event.addArgument(iter.getNext());
		}
		bool result = m_event.call();
		return m_keepRunning;
	};

	void onTweenComplete()
	{
		m_event.beginCall();
		if (!m_id.empty())
			m_event.addArgument(m_id);
		m_event.addArgument(gkString("complete"));
		m_event.addArgument((int)m_tweenParam.properties.size());
		m_event.call();
		m_tweenParam.cleanProperties();
		m_running = false;
	}

	void stop()
	{
		m_keepRunning = false;
	}

	void clearValues() {
		m_initValues.clear();
	}


	void setTransition(short trans)
	{
		m_tweenParam.transition = trans;
	}
	void setEquation(short eq)
	{
		m_tweenParam.equation=eq;
	}
	void setDelay(float delay)
	{
		m_tweenParam.delay=delay;
	}

	void setDelayFinished(float delay)
	{
		m_tweenParam.delayFinished=delay;

	}
	bool isStarted()
	{
		return m_tweenParam.started;
	}
	void setReverse(bool r)
	{
		m_tweenParam.reverse = r;
	}
	void setInfinite(bool i)
	{
		m_tweenParam.setInfinite(i);
	}

	void setRepeat(int repeat)
	{
		m_tweenParam.repeat=repeat;
	}
	void setRepeatWithReverse(int count)
	{
		m_tweenParam.setRepeatWithReverse(count,true);
	}

private:
	typedef std::pair<float,float> Pair;
	typedef utArray<Pair> InitValues;
	typedef utArray<float> TweenValues;

	tween::TweenerParam m_tweenParam;

	float		 m_time;

	InitValues  m_initValues;
	TweenValues  m_tvalues;

	gkLuaEvent	 m_event;
	bool		 m_keepRunning;
	bool		 m_running;
	gkString	 m_id;
};





class gsProcedural {
public:
	gkGameObject* createBox(const gkString& name,Ogre::Real sizeX=1.f, Ogre::Real sizeY=1.f, Ogre::Real sizeZ=1.f, unsigned int numSegX=1, unsigned int numSegY=1, unsigned int numSegZ=1);


	gkGameObject* createRoundedBox(const gkString& name,Ogre::Real sizeX=1.f, Ogre::Real sizeY=1.f, Ogre::Real sizeZ=1.f, unsigned int numSegX=1, unsigned int numSegY=1, unsigned int numSegZ=1,float mChamferSize=0.1f);
};

extern void addResourceLocation(const gkString& group, const gkString& type, const gkString& path, bool recursive=false);
extern void loadResource(const gkString& group,const gkString& path);

void loadBlend(const gkString& name, const gkString& sceneName="",const gkString& groupName="",bool ignoreLibraries=true);
void unloadBlendFile(const gkString& name);
void unloadAllBlendFiles();

#ifdef OGREKIT_COMPILE_LIBCURL
	#ifndef SWIG
class gsDownloadCallback : public gkHttpSessionCallback
{
public:
	gsDownloadCallback(gsSelf self,gsFunction func) : m_event(self,func){}
	gsDownloadCallback(gsFunction func) : m_event(func){}
	virtual void onDone(gkHttpSessionCallback::DoneType type,utStream* stream,const gkString& url);
private:
	gkLuaEvent m_event;
};
	#endif
#endif


// stream mapping, intended as read-only (for now)
class gsLuaStream
{
public:
	gsLuaStream(utMemoryStream stream) : m_stream(stream) {}
	~gsLuaStream() {}
	gkString read(int amount);
	int pos() {return m_stream.position();};
	int size() { return m_stream.size();};
private:
	utMemoryStream m_stream;
};



void downloadFile(const gkString& name, const gkString& to,const gkString& postData="",bool overwrite=true, bool checkPeerCA=false, bool checkPeerHostname=false);
gkString downloadToString(bool blocking,const gkString& url, const gkString& postData="",bool checkPeerCA=false, bool checkPeerHostname=false);
void nbDownload();
void nbPoll();
void nbSetCallback(gsSelf self,gsFunction func);
void nbSetCallback(gsFunction func);
bool nbIsRunning();


extern void sendMessage(const gkString& from,const gkString& to,const gkString& subject,const char* body);

extern void sendEventMessage(const gkString& type, const gkString& value="",const gkString& ext="", bool queued=true);

extern void gsDebugPrint(const char* str);

extern void infoClear(void);
extern void infoLock(bool lockIt);
extern void infoShow(bool showIt);

extern gkString getScriptFolder();




extern bool gsSetCompositorChain(gsCompositorOp op, const gkString& compositorName);

extern bool import(const gkString& scriptName,bool multiCall=false);
extern bool fileExists(const gkString& filename);
extern gkString fileLoad(const gkString& filename);
extern gkString getPlatform();

extern bool isMobile();

extern bool isSoundAvailable();

extern bool isMaterialInitialized(const gkString& matName);
extern void initMaterial(const gkString& matName);

extern gkString getWindowTitle();

extern void setMaterialParam(const gkString& matName, int shaderType,const gkString& paramName, float value);
extern void setMaterialParam(const gkString& matName, int shaderType,const gkString& paramName, int value);
extern void setMaterialParam(const gkString& matName, int shaderType,const gkString& paramName, gsVector3* vec3);
extern void setMaterialParam(const gkString& matName, int shaderType,const gkString& paramName, gsVector4* vec4);

extern void log(const gkString& log);
extern void logDisable();
extern void logEnable(bool verbose=true);

extern gkString md5(const gkString& md5);
extern gkString blowfishEncrypt(const gkString& key, const gkString& in);
extern gkString blowfishDecrypt(const gkString& key, const gkString& in);
extern bool isNetworkAvailable(const gkString& url="http://www.googlemail.com");
extern void asyncIsNetworkAvailable(gsSelf self,gsFunction func,const gkString& url="http://www.googlemail.com");
void ribbon(gsGameObject* gobj,float width=5,float widthChange=2,int length=400,int maxElements=80,const gkString& materialName = "Examples/LightRibbonTrail",gsVector3 initCol=gsVector3(1.0, 0.8, 0),gsVector4 changeCol=gsVector4(0.5,0.5,0.5,0.5));
extern int scancodeToInt(gkScanCode code);
extern unsigned long getTime();
extern unsigned long getSystemTime();

extern gsVector2 getScreenSize();
extern void setScreenSize(int width,int height);

extern float getFPS();

#ifndef SWIG
extern int native_getScreenshotAsString(lua_State*L);
#endif

extern gkString getDataDirectory(const gkString& folder="");
extern gkString getWorkingFolder();

extern gkString compressString(const gkString& inputString, gsCompressionLevel compLevel);
extern const char* compressCString(char* input,int& len, gsCompressionLevel compLevel);
extern gkString uncompressString(const gkString& data);

extern void executeScript(const gkString& luaScript);
extern void setSinglestepMode(bool singlestepMode);
extern bool inSinglestepMode();
extern void requestSinglestep();


#endif//_OgreKitApi_h_
