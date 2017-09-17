/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2010 Charlie C.

    Contributor(s): Jonathan.
    				Thomas Trocha(dertom)
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
#include <Events/gkEvents.h>
#include <gkEventManager.h>
#include <rapidjson/document.h>
#include <time.h>
#include "gsCore.h"
#include "OgreHlmsManager.h"
#include "OgreHlmsPbsMaterial.h"
#include "gsPhysics.h"
#include "gkCam2ViewportRay.h"
#include "gkMesh.h"
#include "External/Ogre/gkOgreMaterialLoader.h"
#include "gkStats.h"
#include "gkEntity.h"
#include "OgreEntity.h"
#include "gsAI.h"

#include <string>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>

#include <zlib.h>

# include "../../../Ogre-1.9a/External/OgreProcedural/library/include/Procedural.h"
# include "../../../Ogre-1.9a/External/OgreProcedural/library/include/ProceduralUtils.h"

#include "../../../Ogre-1.9a/External/OgreProcedural/library/include/ProceduralPlaneGenerator.h"
#include "../../../Ogre-1.9a/External/OgreProcedural/library/include/ProceduralSphereGenerator.h"
#include "../../../Ogre-1.9a/External/OgreProcedural/library/include/ProceduralCylinderGenerator.h"
#include "../../../Ogre-1.9a/External/OgreProcedural/library/include/ProceduralTorusGenerator.h"
#include "../../../Ogre-1.9a/External/OgreProcedural/library/include/ProceduralConeGenerator.h"
#include "../../../Ogre-1.9a/External/OgreProcedural/library/include/ProceduralPath.h"
#include "../../../Ogre-1.9a/External/OgreProcedural/library/include/ProceduralMultiShape.h"
#include "../../../Ogre-1.9a/External/OgreProcedural/library/include/ProceduralSVG.h"
#include "../../../Ogre-1.9a/External/OgreProcedural/library/include/ProceduralExtruder.h"
#include "../../../Ogre-1.9a/External/OgreProcedural/library/include/ProceduralBoxGenerator.h"
#include "../../../Ogre-1.9a/External/OgreProcedural/library/include/ProceduralRoundedBoxGenerator.h"
#include "../../gkCrypt.h"

#include "gkWindowSystem.h"
#include "gkPath.h"

#include "gkWindow.h"

#include "OISMultiTouch.h"

#include "OgrePrerequisites.h"

#include "OgreMaterialManager.h"

#include "OgreRectangle2D.h"

#include "OgreAxisAlignedBox.h"

#include "OgreSceneManager.h"

#include "OgreSceneNode.h"

#include "gkScene.h"

#include "OgreHardwareVertexBuffer.h"
#ifdef OGREKIT_COMPILE_LIBCURL
# include "Network/gkDownloadManager.h"
#endif

#if GK_PLATFORM == GK_PLATFORM_ANDROID
# include "android/AndroidInputManager.h"
# include "gkWindowAndroid.h"
#endif

#ifdef OGREKIT_USE_OPENSSL
# include "openssl/md5.h"
#include "../Lua/gkLuaUtils.h"
#include "../../gkValue.h"
#endif
gsProperty::gsProperty(gkVariable* var) : m_prop(var), m_creator(false)
{
}


gsProperty::gsProperty() : m_prop(0), m_creator(false)
{
}


gsProperty::gsProperty(const gkString& name, bool value)
{
	m_creator = true;
	m_prop = new gkVariable(name, false);
	m_prop->setValue(value);
}


gsProperty::gsProperty(const gkString& name, double value)
{
	m_creator = true;
	m_prop = new gkVariable(name, false);
	m_prop->setValue((float)value);
}



gsProperty::gsProperty(const gkString& name, const gkString& value)
{
	m_creator = true;
	m_prop = new gkVariable(name, false);
	m_prop->setValue(value);
}



gsProperty::gsProperty(const gsProperty& oth)
{
	m_creator = true;
	m_prop = new gkVariable(oth.getName(), false);
	m_prop->setValue(oth.getValue());
}


gsProperty::~gsProperty()
{
	makeDebug(false);
	if (m_creator)
		delete m_prop;
}



const gkString& gsProperty::getName(void) const
{
	static gkString localStr = "";
	if (m_prop)
		return m_prop->getName();
	return localStr;
}



const gkString& gsProperty::getValue(void) const
{
	static gkString localStr;
	if (m_prop)
		localStr = m_prop->getValueString();
	return localStr;
}



void gsProperty::makeDebug(bool v)
{
	if (m_prop && m_prop->isDebug() != v && gkEngine::getSingletonPtr())
	{
		m_prop->setDebug(v);
		if (v)
			gkEngine::getSingleton().addDebugProperty(m_prop);
		else
			gkEngine::getSingleton().removeDebugProperty(m_prop);
	}
}



gsPropertyType gsProperty::getType(void)  const
{

	if (m_prop)
	{
		switch (m_prop->getType())
		{
		case gkVariable::VAR_BOOL:
			return PROP_BOOL;
		case gkVariable::VAR_INT:
		case gkVariable::VAR_REAL:
			return PROP_NUMBER;
			// TODO
		default:
			break;
		}
		return PROP_STRING;
	}
	return PROP_NULL;
}



bool gsProperty::toBool(void)  const
{
	if (m_prop)
		return m_prop->getValueBool();
	return false;
}



double gsProperty::toNumber(void)  const
{
	if (m_prop)
		return (double)m_prop->getValueReal();
	return -1;

}



gkString gsProperty::toString(void)  const
{
	if (m_prop)
		return m_prop->getValueString();
	return "";
}

gsVector3 gsProperty::toVec3(void) const
{
	if (m_prop)
		return gsVector3(m_prop->getValueVector3());
	return gsVector3(0,0,0);
}

void gsProperty::fromBool(bool v)
{
	if (m_prop)
		m_prop->setValue(v);
}



void gsProperty::fromNumber(double v)
{
	if (m_prop)
		m_prop->setValue((gkScalar)v);
}



void gsProperty::fromString(const gkString& v)
{
	if (m_prop)
		m_prop->setValue(v);
}

void gsProperty::fromVector3(gsVector3 v)
{
	if (m_prop)
		m_prop->setValue(v);
}

#define PROP_INSERT_T(name, var, cast)\
    m_properties.insert(name, new gsProperty(name, (cast)m_defs->var));

#define PROP_INSERT_N(name, var) PROP_INSERT_T(name, var, double)
#define PROP_INSERT_B(name, var) PROP_INSERT_T(name, var, bool)
#define PROP_INSERT_S(name, var) PROP_INSERT_T(name, var, gkString)
#define PROP_INSERT_VEC(name, var) PROP_INSERT_T(name, var, gsVector3)

static void gkVar_fromVar(gkVariable& v, gsVector3& r)
{
	r = v.getValueVector3();
}


static void gkVar_fromVar(gkVariable& v, bool& r)
{
	r = v.getValueBool();
}

static void gkVar_fromVar(gkVariable& v, int& r)
{
	r = v.getValueInt();
}
static void gkVar_fromVar(gkVariable& v, gkScalar& r)
{
	r = v.getValueReal();
}
static void gkVar_fromVar(gkVariable& v, gkString& r)
{
	r = v.getValueString();
}


#define PROP_SET_T(name, var, cast)\
    if (v.getName() == name )\
        gkVar_fromVar(v, (cast)m_defs->var);

#define PROP_SET_TE(name, var, cast)\
    if (v.getName() == name )\
    {\
        int c = (int)m_defs->var;\
        gkVar_fromVar(v, c);\
        m_defs->var = (cast)c;\
    }

#define PROP_SET_I(name, var) PROP_SET_T(name, var, int&)
#define PROP_SET_N(name, var) PROP_SET_T(name, var, gkScalar&)
#define PROP_SET_B(name, var) PROP_SET_T(name, var, bool&)
#define PROP_SET_S(name, var) PROP_SET_T(name, var, gkString&)
#define PROP_SET_VEC(name, var) PROP_SET_T(name, var, gsVector3&)
#define PROP_SET_E(name, var, E) PROP_SET_TE(name, var, E)





gsUserDefs::gsUserDefs(gkUserDefs* defs)
{
	m_defs = defs;

	PROP_INSERT_N("renderSystem",       rendersystem);
	PROP_INSERT_N("sceneManager",       sceneManager);
	PROP_INSERT_B("verbose",            verbose);
	PROP_INSERT_N("winx",               winsize.x);
	PROP_INSERT_N("winy",               winsize.y);
	PROP_INSERT_S("winTitle",           wintitle);
	PROP_INSERT_B("fullScreen",         fullscreen);
	PROP_INSERT_B("blenderMat",         blendermat);
	PROP_INSERT_B("matblending",		matblending);
	PROP_INSERT_B("grabInput",          grabInput);
	PROP_INSERT_B("escToExit",          esctoexit);
	PROP_INSERT_B("debugFps",           debugFps);
	PROP_INSERT_B("debugPhysics",       debugPhysics);
	PROP_INSERT_B("debugPhysicsAABB",   debugPhysicsAabb);
	PROP_INSERT_B("usebulletDBVT",      useBulletDbvt);
	PROP_INSERT_B("showDebugProps",     showDebugProps);
	PROP_INSERT_B("debugSounds",        debugSounds);
	PROP_INSERT_B("enableShadows",      enableshadows);
	PROP_INSERT_S("shadowTechnique",    shadowtechnique);
	PROP_INSERT_N("colourShadowR",      colourshadow.r);
	PROP_INSERT_N("colourShadowG",      colourshadow.g);
	PROP_INSERT_N("colourShadowB",      colourshadow.b);
	PROP_INSERT_N("farDistanceShadow",  fardistanceshadow);
}




gsUserDefs::~gsUserDefs()
{
	UTsize p = 0;
	for (; p < m_properties.size(); ++p)
		delete m_properties.at(p);

	m_properties.clear();
}



void gsUserDefs::setValueEvent(gkVariable& v)
{
	if (m_properties.find(gkHashedString(v.getName())) != UT_NPOS)
	{
		PROP_SET_E("renderSystem",       rendersystem, OgreRenderSystem);
		PROP_SET_I("sceneManager",       sceneManager);
		PROP_SET_B("verbose",            verbose);
		PROP_SET_N("winx",               winsize.x);
		PROP_SET_N("winy",               winsize.y);
		PROP_SET_S("winTitle",           wintitle);
		PROP_SET_B("fullScreen",         fullscreen);
		PROP_SET_B("blenderMat",         blendermat);
		PROP_SET_B("grabInput",          grabInput);
		PROP_SET_B("escToExit",          esctoexit);
		PROP_SET_B("debugFps",           debugFps);
		PROP_SET_B("debugPhysics",       debugPhysics);
		PROP_SET_B("debugPhysicsAABB",   debugPhysicsAabb);
		PROP_SET_B("usebulletDBVT",      useBulletDbvt);
		PROP_SET_B("showDebugProps",     showDebugProps);
		PROP_SET_B("debugSounds",        debugSounds);
		PROP_SET_B("enableShadows",      enableshadows);
		PROP_SET_S("shadowTechnique",    shadowtechnique);
		PROP_SET_N("colourShadowR",      colourshadow.r);
		PROP_SET_N("colourShadowG",      colourshadow.g);
		PROP_SET_N("colourShadowB",      colourshadow.b);
		PROP_SET_N("farDistanceShadow",  fardistanceshadow);
	}
}




const gsProperty& gsUserDefs::getProperty(const gkString& name)
{
	return __getitem__(name.c_str());
}




void gsUserDefs::addProperty(const gsProperty& prop)
{
	if (!prop.getName().empty())
	{
		if (m_properties.find(prop.getName()) == UT_NPOS)
			m_properties.insert(prop.getName(), new gsProperty(prop));
	}
}



bool gsUserDefs::hasProperty(const gkString& name)
{
	return m_properties.find(name) != UT_NPOS;
}



const gsProperty& gsUserDefs::__getitem__(const char* name)
{
	static gsProperty* prop = 0;
	UTsize pos = 0;
	if ((pos = m_properties.find(gkHashedString(name))) != UT_NPOS)
	{
		delete prop;
		prop = new gsProperty((const gsProperty&)(*(m_properties.at(pos))));
	}
	else
	{
		if (prop)
			delete prop;
		prop = new gsProperty();
	}
	return *prop;
}



void gsUserDefs::__setitem__(const char* name, bool  v)
{
	UTsize pos = 0;
	if ((pos = m_properties.find(gkHashedString(name))) != UT_NPOS)
	{
		(m_properties.at(pos))->fromBool(v);
		setValueEvent(m_properties.at(pos)->getClassRef());
	}
}



void gsUserDefs::__setitem__(const char* name, double v)
{
	UTsize pos = 0;
	if ((pos = m_properties.find(gkHashedString(name))) != UT_NPOS)
	{
		(m_properties.at(pos))->fromNumber(v);
		setValueEvent(m_properties.at(pos)->getClassRef());
	}
}



void gsUserDefs::__setitem__(const char* name, const gkString& v)
{
	UTsize pos = 0;
	if ((pos = m_properties.find(gkHashedString(name))) != UT_NPOS)
	{
		(m_properties.at(pos))->fromString(v);
		setValueEvent(m_properties.at(pos)->getClassRef());
	}
}

gsTouch::gsTouch()
{
	m_touch = gkWindowSystem::getSingleton().getMainWindow()->getInputManager()->_getMultiTouch();
}

gsTouch::~gsTouch()
{
	TouchListeners::Iterator iter(m_listeners);
	while(iter.hasMoreElements())
	{
		delete iter.getNext();
	}
}

int gsTouch::getNumStates()
{
	if (m_touch)
		return m_touch->getNumStates();

	return 0;
}

gkMultiTouchState* gsTouch::getState(int i)
{
	if (m_touch && i < m_touch->getNumStates()){
		gkMultiTouchState& st = m_touch->getMultiTouchState(i);
		return &st;
	}

	return &m_nullState;
}

void gsTouch::addListener(gsSelf self,gsFunction func)
{
#if GK_PLATFORM == GK_PLATFORM_ANDROID
	gsTouch::gsTouchListener* listener = new gsTouch::gsTouchListener(self,func);
	static_cast<gkWindowAndroid*>(gkWindowSystem::getSingleton().getMainWindow())->addMultiTouchCallback(listener);
	// keep ref for destruction
	m_listeners.push_back(listener);
#else
	gkDebugScreen::printTo("gsTouch::addListener not supported on this platform!");
#endif
}

void gsTouch::addListener(gsFunction func)
{
#if GK_PLATFORM == GK_PLATFORM_ANDROID
	gsTouch::gsTouchListener* listener = new gsTouch::gsTouchListener(func);
	static_cast<gkWindowAndroid*>(gkWindowSystem::getSingleton().getMainWindow())->addMultiTouchCallback(listener);
	// keep ref for destruction
	m_listeners.push_back(listener);
#else
	gkDebugScreen::printTo("gsTouch::addListener not supported on this platform!");
#endif
}



gsMouse::gsMouse()
	:    xpos(0), ypos(0),
	     xrel(0), yrel(0),
	     winx(0), winy(0),
	     wheel(0), moved(false)
{
}



gsMouse::~gsMouse()
{
}



void gsMouse::capture(void)
{
	if (!gkWindowSystem::getSingletonPtr()) return;

	gkMouse* mse = gkWindowSystem::getSingleton().getMouse();
	xpos    = mse->position.x;
	ypos    = mse->position.y;
	xrel    = mse->relative.x;
	yrel    = mse->relative.y;
	winx    = mse->winsize.x;
	winy    = mse->winsize.y;
	wheel   = mse->wheelDelta;
	moved   = mse->moved;
}


bool gsMouse::isButtonDown(int btn)
{
	if (!gkWindowSystem::getSingletonPtr()) return false;
	return gkWindowSystem::getSingleton().getMouse()->isButtonDown((gsMouseButton)btn);
}



int getNumJoysticks(void)
{
	if (!gkWindowSystem::getSingletonPtr()) return 0;
	return gkWindowSystem::getSingleton().getNumJoysticks();
}

gsJoystick::gsJoystick(int i)
	: m_joystick(1,3), m_index(i)
{
	GK_ASSERT(i >= 0);
	GK_ASSERT(i < getNumJoysticks());
}

gsJoystick::~gsJoystick()
{
}

void gsJoystick::capture(void)
{
	GK_ASSERT(m_index < getNumJoysticks());

	if(m_index >= 0 && m_index < getNumJoysticks())
		m_joystick = *gkWindowSystem::getSingleton().getJoystick(m_index);
}

int gsJoystick::getNumAxes()
{
	return m_joystick.getAxesNumber();
}

int gsJoystick::getAxis(int i)
{
	return m_joystick.getAxisValue(i);
}

int gsJoystick::getRelAxis(int i)
{
	return m_joystick.getRelAxisValue(i);
}

int gsJoystick::getNumButtons()
{
	return m_joystick.getButtonsNumber();
}

int gsJoystick::getButtonCount()
{
	return m_joystick.buttonCount;
}

bool gsJoystick::isButtonDown(int i)
{
	return m_joystick.isButtonDown(i);
}

bool gsJoystick::wasButtonPressed(int i)
{
	return m_joystick.wasButtonPressed(i);
}

gsVector3 gsJoystick::getAccel()
{
	const gkVector3& a = m_joystick.accel;
	return gsVector3(a.x, a.y, a.z);
}

int gsJoystick::getWinWidth(void)
{
	if (!gkWindowSystem::getSingletonPtr()) return 0;
	return gkWindowSystem::getSingleton().getMainWindow()->getWidth();
}

int gsJoystick::getWinHeight(void)
{
	if (!gkWindowSystem::getSingletonPtr()) return 0;
	return gkWindowSystem::getSingleton().getMainWindow()->getHeight();
}



gsKeyboard::gsKeyboard() : m_event(0)
{
	gkWindowSystem::getSingleton().addListener(this);
}


gsKeyboard::~gsKeyboard()
{
//	gkWindowSystem::getSingleton().removeListener(this);
	if (m_event)
		delete m_event;
}



bool gsKeyboard::isKeyDown(int sc)
{
	if (!gkWindowSystem::getSingletonPtr()) return false;
	return gkWindowSystem::getSingleton().getKeyboard()->isKeyDown((gkScanCode)sc);
}

bool gsKeyboard::isKeyUp(int sc)
{
	if (!gkWindowSystem::getSingletonPtr()) return false;
	return gkWindowSystem::getSingleton().getKeyboard()->isKeyUp((gkScanCode)sc);
}

void gsKeyboard::clearKey(int sc)
{
	if (!gkWindowSystem::getSingletonPtr()) return;
	return gkWindowSystem::getSingleton().getKeyboard()->clearKey((gkScanCode)sc);
}


void gsKeyboard::setCallback(gsSelf self,gsFunction func)
{
	if (m_event)
		delete m_event;

	m_event = new gkLuaEvent(self,func);
}

void gsKeyboard::setCallback(gsFunction func)
{
	if (m_event)
		delete m_event;

	m_event = new gkLuaEvent(func);
}

void gsKeyboard::keyPressed(const gkKeyboard& key, const gkScanCode& sc)
{
	if (m_event)
	{
		m_event->beginCall();
		m_event->addArgument("pressed");
		int code = ((int)sc)-1;
		m_event->addArgument(code);
		if (code>=0 && code < 87)
		{
			m_event->addArgument(gkString("")+(char)key.text);
		}
		m_event->call();
	}
}
void gsKeyboard::keyReleased(const gkKeyboard& key, const gkScanCode& sc)
{
	if (m_event)
	{
		m_event->beginCall();
		m_event->addArgument("released");
		int code = ((int)sc)-1;
		m_event->addArgument(code);
		if (code>0 && code < 87)
		{
			m_event->addArgument(gkString("")+(char)key.text);
		}
		m_event->call();
	}
}


gsEngine::gsEngine()
	: m_isSystemEngine(false)
//: block(false), executingScript(false)
{
	m_singleton = this;
	m_defs = 0;
	m_running = false;
	if (m_ctxOwner = ((m_engine = gkEngine::getSingletonPtr()) == 0))
		m_engine = new gkEngine();
	else
	{
		m_engine = gkEngine::getSingletonPtr();
		m_running = m_engine->isRunning();
	}

	// TODO: That is not optimal! You would have a listener for every call! Maybe you
	//       should tell in the constructor if you want a new listener added!?
	if (m_engine && !m_engine->isScriptListenerActive())
		m_engine->addListener(this);
}



gsEngine::~gsEngine()
{
	if (m_defs)
		delete m_defs;


	for (UTsize i = 0; i < m_ticks.size(); ++i)
		delete m_ticks.at(i);

	if (m_engine)
		m_engine->removeListener(this);

	if (m_ctxOwner)
	{
		m_running = false;
		delete m_engine;
	}

    m_singleton = 0;
}


gsEngine* gsEngine::m_singleton = 0;

void gsEngine::connect(int evt, gsFunction func)
{
	if (evt == EVT_TICK)
		m_ticks.push_back(new gkLuaEvent(func));
}




void gsEngine::connect(int evt, gsSelf self, gsFunction method)
{
	if (evt == EVT_TICK)
		m_ticks.push_back(new gkLuaEvent(self, method));
}



void gsEngine::tick(gkScalar delta)
{
	gkStats::getSingleton().startClock();

	UTsize i;


//	while (block) {
//		m_fsync.wait();
//	}
//
//	executingScript = true;

	if (!m_engine->inSinglestepMode() || m_engine->m_doStep || this->isSystemMode())
	{
		for (i = 0; i < m_ticks.size(); ++i)
		{

			gkLuaEvent* ob = m_ticks.at(i);

			ob->beginCall();
			ob->addArgument(delta);

			if (!ob->call())
			{
	//			m_ticks.erase(i);
	//			delete ob;
	//
	//			if (i > 0)
	//			{
	//				i -= 1;
	//			}
	//			else
	//				break;
	//			continue;
			}
		}
	}
//	executingScript = false;
//	m_scriptSync.signal();

	gkStats::getSingleton().stopLuaScriptClock();
}

gsEngine& gsEngine::getSingleton(void) {                      \
     return *m_singleton;                            \
 }                                                   \
 gsEngine* gsEngine::getSingletonPtr(void){                    \
     return m_singleton;                             \
 }


//UT_IMPLEMENT_SINGLETON(gsEngine);

void gsEngine::initialize(void)
{
	if (m_ctxOwner && m_engine && !m_engine->isInitialized())
		m_engine->initialize();
}




void gsEngine::requestExit(void)
{
	if (m_engine)
		m_engine->requestExit();
}




void gsEngine::saveTimestampedScreenShot(const gkString& filenamePrefix, const gkString& filenameSuffix)
{
	if (m_engine)
		m_engine->saveTimestampedScreenShot(filenamePrefix, filenameSuffix);
}




gsUserDefs& gsEngine::getUserDefs(void)
{
	if (m_defs == 0)
		m_defs = new gsUserDefs(&m_engine->getUserDefs());
	return *m_defs;
}

void gsEngine::loadBlendFile(const gkString& name, const gkString& sceneName,const gkString& groupName,bool ignoreLibraries)
{
	loadBlend(name,sceneName,groupName,ignoreLibraries);
}




gkScene* gsEngine::getActiveScene(void)
{
	if (m_engine && m_engine->isInitialized())
		return m_engine->getActiveScene();
	return 0;
}

gkScene* gsEngine::getScene(const gkString& sceneName,const gkString& groupName)
{
	return static_cast<gkScene*>(gkSceneManager::getSingleton().getByName(gkResourceName(sceneName,groupName)));
}


gkScene* gsEngine::createEmptyScene(const gkString& sceneName,const gkString& camName, const gkString& groupName)
{
	gkScene* scene = gkSceneManager::getSingleton().createEmptyScene(sceneName,camName,groupName);
	return scene;
}

gkScene* gsEngine::addOverlayScene(gsScene* scene)
{
	if (scene)
		return addOverlayScene(scene->getOwner());
	return 0;
}


gkScene* gsEngine::addOverlayScene(const gkString& sceneName)
{
	gkScene* scene = 0;

	scene = static_cast<gkScene*>(gkSceneManager::getSingleton().getByName(sceneName));
	addOverlayScene(scene);
	return scene;
}

gkScene* gsEngine::addOverlayScene(gkScene* scene)
{
	if (scene && !scene->isInstanced())
	{
		gkWindow* win;
		win = m_engine->getActiveScene()->getDisplayWindow();
		int zorder = win->getViewport(win->getViewportCount()-1)->getZOrder();
		scene->destroyInstance(true);
		scene->setDisplayWindow(win, zorder+1);
		scene->createInstance(true);
	}
	return scene;
}

gkScene* gsEngine::addBackgroundScene(const gkString& sceneName)
{
	gkScene* scene = 0;
	scene = (gkScene*)gkSceneManager::getSingleton().getByName(sceneName);
	addBackgroundScene(scene);
	return scene;
}

gkScene* gsEngine::addBackgroundScene(gsScene* scene)
{
	if (scene)
		return addBackgroundScene(scene->getOwner());
	return 0;
}

gkScene*  gsEngine::addBackgroundScene(gkScene* scene)
{
	if (scene && !scene->isInstanced())
	{
		gkWindow* win;
		win = m_engine->getActiveScene()->getDisplayWindow();
		int zorder = win->getViewport(0)->getZOrder();
		scene->setDisplayWindow(win, zorder-1);
		scene->createInstance(true);
	}
	return scene;
}


void gsEngine::removeScene(const gkString& m_sceneName)
{
	gkScene* scene = 0;
	scene = (gkScene*)gkSceneManager::getSingleton().getByName(m_sceneName);
	removeScene(scene);
}

void  gsEngine::removeScene(gkScene* scene)
{
	if (scene)
		scene->destroyInstance(false);
}

void  gsEngine::removeScene(gsScene* scene)
{
	if (scene)
		removeScene(scene->getOwner());
}

void gsEngine::run(void)
{
	if (!m_running && m_engine)
	{
		m_running = true;
		m_engine->run();
	}
}





gsHUD::gsHUD() : m_object(0)
{
}

gsHUD::gsHUD(gkHUD* ob) : m_object(ob)
{
}

void gsHUD::show(bool v)
{
	if (m_object)
		m_object->show(v);
}

gkHUDElement* gsHUD::getChild(const gkString& child)
{
	if (m_object)
	{
		gkHUDElement* ob = m_object->getChild(child);
		return ob;			
	}

	return 0;
}

gsHUDElement::gsHUDElement() : m_object(0)
{
}

gsHUDElement::gsHUDElement(gkHUDElement* ob) : m_object(ob)
{
}

void gsHUDElement::show(bool v)
{
	if (m_object)
		m_object->show(v);
}

gkString gsHUDElement::getValue()
{	
	return m_object ? m_object->getValue() : "";
}

void gsHUDElement::setValue(const gkString& value)
{
	if (m_object)
		m_object->setValue(value);
}

void gsHUDElement::setUvCoords(float u1, float v1, float u2, float v2)
{
	if (m_object)
		m_object->setUvCoords(u1, v1, u2, v2);
}

gkString gsHUDElement::getMaterialName()
{
	if (m_object)
		return m_object->getMaterialName();
	
	return "";
}

void gsHUDElement::setMaterialName(const gkString& material)
{
	if (m_object)
		m_object->setMaterialName(material);
}

float gsHUDElement::getMaterialAlpha()
{
	if (m_object)
		return m_object->getMaterialAlpha();

	return 1.f;
}

int gsHUDElement::getMaterialAlphaRejectValue()
{
	if (m_object)
		return m_object->getMaterialAlphaRejectValue();

	return 0;
}

void gsHUDElement::setMaterialAlphaRejectValue(int val, bool isGreater)
{
	if (m_object)
		m_object->setMaterialAlphaRejectValue(val, isGreater);
}

void gsHUDElement::setMaterialAlpha(float factor)
{
	if (m_object)
		m_object->setMaterialAlpha(factor);
}
	
gkString gsHUDElement::getParameter(const gkString& name)
{
	if (m_object)
		return m_object->getParameter(name);

	return "";
}

void gsHUDElement::setParameter(const gkString& name, const gkString& value)
{
	if (m_object)
		return m_object->setParameter(name, value);
}

gsVector3 gsHUDElement::getPosition()
{
	if (m_object){
		const gkVector2& vec2 = m_object->getPosition();
		mPosition.x = vec2.x;
		mPosition.y = vec2.y;
	}
	return mPosition;
}

void gsHUDElement::setPosition(gsVector3 vec)
{
	setPosition(vec.x,vec.y);
}

void gsHUDElement::setPosition(float x,float y)
{
	if (m_object)
		m_object->setPosition(x,y);
}

gsObject::gsObject() : m_object(0)
{
}



gsObject::gsObject(gkInstancedObject* ob) : m_object(ob)
{
}



void gsObject::createInstance(void)
{
	if (m_object)
	{
		gkEngine* eng = gkEngine::getSingletonPtr();
		m_object->createInstance(eng->isRunning());
	}
}



void gsObject::destroyInstance(void)
{
	if (m_object)
	{
		gkEngine* eng = gkEngine::getSingletonPtr();
		m_object->destroyInstance(eng->isRunning());
	}
}


void gsObject::reinstance(void)
{
	if (m_object)
	{
		gkEngine* eng = gkEngine::getSingletonPtr();
		m_object->reinstance(eng->isRunning());
	}
}

gkString gsObject::getName(void)
{
	if (m_object)
		return m_object->getName();
	return "";
}

gkString gsObject::getGroupName(void)
{
	return m_object?m_object->getGroupName()
				    :"";
}



gsScene::gsScene() : m_pickRay(0)
#ifdef OGREKIT_USE_PROCESSMANAGER
	, m_processManager(0),m_hlmsManager(0)
#endif
{
}

gsScene::gsScene(gkInstancedObject* ob) : gsObject(ob),m_pickRay(0),m_hlmsManager(0)
#ifdef OGREKIT_USE_PROCESSMANAGER
	,m_processManager(0)
#endif
{
}



gsScene::~gsScene()
{
	if (m_pickRay){
		delete m_pickRay;
	}

	StaticBatches::Iterator iter(mStaticBatches);
	while (iter.hasMoreElements())
	{
		gsStaticBatch* stb = iter.getNext();
		delete stb;
	}
	mStaticBatches.clear();
}


void gsScene::setPBS(float f0,float r,float lr,const gkString& entName,const gkString& albedo,const gkString& normal,const gkString& envmap)
{
	gkGameObject* gobj = gkEngine::getSingleton().getActiveScene()->getObject(entName);






	if (gobj)
	{
		Ogre::SceneManager* mSceneManager = gkEngine::getSingleton().getActiveScene()->getManager();
		Ogre::Entity* ent = gkEngine::getSingleton().getActiveScene()->getObject(entName)->getEntity()->getEntity();
//		Ogre::Entity *ent = mSceneManager->createEntity("Sphere1000.mesh");
//		Ogre::SceneNode *sceneNode = mSceneManager->getRootSceneNode()->createChildSceneNode();
//		sceneNode->setPosition(Ogre::Vector3(1.0f * 3 - 4,	1.0f, 1.0f * 2 - 4));
//		sceneNode->attachObject(ent);




		if (!m_hlmsManager)
		{
			m_hlmsManager = new Ogre::HlmsManager(mSceneManager);
		}


		Ogre::PbsMaterial* pbsMaterial = new Ogre::PbsMaterial();
		pbsMaterial->setAlbedo(Ogre::ColourValue::Green);

		pbsMaterial->setRoughness(r);
		pbsMaterial->setLightRoughnessOffset(lr);


//		pbsMaterial->setF0(Ogre::ColourValue(f0, f0, f0));

		if (!envmap.empty())
		{
			Ogre::TexturePtr cubeMap = Ogre::TextureManager::getSingletonPtr()->load(envmap, Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_CUBE_MAP);
			pbsMaterial->setEnvironmentMap(cubeMap);
		}


		if (!albedo.empty())
		{
			Ogre::TexturePtr diffuseMap = Ogre::TextureManager::getSingletonPtr()->load(albedo, Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D);
			pbsMaterial->setAlbedoTexture(Ogre::PbsMaterial::MS_MAIN,diffuseMap);

		}
//		pbsMaterial->set
		if (!normal.empty())
		{
			Ogre::TexturePtr normalMap = Ogre::TextureManager::getSingletonPtr()->load(normal, Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D);
			pbsMaterial->setNormalrTexture(Ogre::PbsMaterial::MS_MAIN,normalMap);
		}
//		pbsMaterial->setNormalrTexture(Ogre::PbsMaterial::MS_MAIN)




		createHLMSMaterial(ent, pbsMaterial, "PBS_"+entName);
	}



}

void gsScene::createHLMSMaterial(Ogre::Entity *ent, Ogre::PbsMaterial* pbsMaterial, const gkString& matName)
{
	size_t count = ent->getNumSubEntities();
	for (size_t i = 0; i < count; i++)
	{
		Ogre::SubEntity* subEnt = ent->getSubEntity(i);

		Ogre::MaterialPtr newMat = subEnt->getMaterial()->clone(matName + "_" + Ogre::StringConverter::toString(i));

		newMat->getBestTechnique()->removeAllPasses();

		Ogre::Pass* pass = newMat->getBestTechnique()->createPass();
		pass->setName("pbs");
//		pbsMaterial->createTexturUnits(pass);

		subEnt->setMaterial(newMat);


	}
}


gkDebugger* gsScene::getDebugger()
{
	return cast<gkScene>()->getDebugger();
}

void gsScene::destroyObject(gsGameObject* gobj)
{
	if (m_object && gobj)
	{
		gkGameObject* destroyObject = gobj->cast<gkGameObject>();
		cast<gkScene>()->destroyObject(destroyObject);
	}
}

void gsScene::setLayer(int layer)
{
	if (m_object)
		return cast<gkScene>()->setLayer((UTuint32)layer);
}

int gsScene::getLayer()
{
	if (m_object)
		return cast<gkScene>()->getLayer();
	return 0;
}

bool gsScene::hasObject(const gkString& name)
{
	if (m_object)
		return cast<gkScene>()->hasObject(name);
	return false;
}

gkScene* gsScene::getOwner()
{
	if (m_object)
	{
		return cast<gkScene>();
	}
	return 0;
}

#ifdef OGREKIT_USE_PROCESSMANAGER
gkProcessManager* gsScene::getProcessManager()
{
	if (m_object)
	{
		if (!m_processManager)
			m_processManager = cast<gkScene>()->getProcessManager();
		return m_processManager;
	}
	return 0;
}
#endif

gkGameObject* gsScene::getObject(const gkString& name)
{
	if (m_object)
	{
		gkGameObject* gobj = cast<gkScene>()->getObject(name);
		if (gobj)
			return gobj;
	}
	return 0;
}


gkGameObject* gsScene::createEmpty(const gkString& name)
{
	if (m_object)
	{
		gkScene* scene = cast<gkScene>();
		if (!scene->hasObject(name))
		{
			gkGameObject* obj = scene->createObject(name);
			return obj;
		}
	}

	return 0;
}

gkLight* gsScene::createLight(const gkString& name, gsLightType type)
{
	if (m_object)
	{
		gkScene* scene = cast<gkScene>();
		if (!scene->hasObject(name))
		{
			gkLight* light = scene->createLight(name);
			gkLightProperties& props = light->getLightProperties();
			props.m_type = type;
			props.m_diffuse = gkColor(1.f,1.f,1.f);
			props.m_linear = 1.f;
			props.m_constant = 1.f;
			props.m_quadratic = 1.f;
			props.m_power = 1.f;
			props.m_specular=gkColor(0.5f,0.5f,0.5f);
			props.m_casts=false;
			props.m_range=1.f;
			return light;
		}
		else
		{
			gkLogger::write("WARNING: There was already an object with the name:" + name + " in the scene! Ignored Light-Creation!",true);
		}
	}
	return 0;
}

gkGameObject* gsScene::createEntity(const gkString& name)
{
	if (m_object)
	{
		gkScene* scene = cast<gkScene>();
		if (!scene->hasObject(name))
		{
			gkEntity* ent = scene->createEntity(name);
			if (!ent)
				return 0;
		    gkMesh* mesh = scene->createMesh("mesh_"+name);
		    gkSubMesh* submesh = new gkSubMesh();
		    //submesh->setVertexColors(true);
		    mesh->addSubMesh(submesh);
		    ent->getEntityProperties().m_mesh = mesh;
			return ent;
		}
	}

	return 0;
}

gkGameObject* gsScene::createCamera(const gkString& name)
{
	if (m_object)
	{
		gkScene* scene = cast<gkScene>();
		if (!scene->hasObject(name))
		{
			gkCamera* cam = scene->createCamera(name);
			if (!cam)
				return 0;
			return cam;
		}
	}

	return 0;
}




gkRecastDebugger* gsScene::getRecastDebugger()
{
	if (m_object)
	{
		gkScene* scene = cast<gkScene>();
		return scene->getRecastDebugger();
	}

	return 0;
}


gkLogicManager* gsScene::getLogicBrickManager()
{
	if (m_object)
	{
		gkScene* scene = cast<gkScene>();
		return scene->getLogicBrickManager();
	}

	return 0;
}

gsRay* gsScene::getPickRay(float rayX,float rayY){
	if (m_object){

		if (rayX==-1 || rayY==-1)
		{
			gkMouse* mse = gkWindowSystem::getSingleton().getMouse();
			rayX = mse->position.x;
			rayY = mse->position.y;
		}

		gkCam2ViewportRay pickRay = gkCam2ViewportRay(rayX, rayY, 10000, cast<gkScene>());

		if (!m_pickRay){
			m_pickRay = new gsRay;
		}
		m_pickRay->setDirection(pickRay.getDirection());
		m_pickRay->setOrigin(pickRay.getOrigin());
		return m_pickRay;
	}
	return 0;
}

gkGameObject* gsScene::cloneObject(gsGameObject* obj, int lifeSpan, bool instantiate)
{
	if (m_object && obj)
	{
        gkScene*      scene   = cast<gkScene>();
		gkGameObject* gameObj = obj->cast<gkGameObject>();
		if (gameObj)
		{
			gkGameObject* newGameObj = scene->cloneObject(gameObj, lifeSpan, instantiate);
            if (newGameObj)
                return newGameObj;
		}
	}
    
	return 0;    
}


gkDynamicsWorld* gsScene::getDynamicsWorld(void)
{
	if (m_object)
	{
		gkScene* scene = cast<gkScene>();
		gkDynamicsWorld* world = scene->getDynamicsWorld();
		return world;
	}

	return 0;
}

gkCamera* gsScene::getMainCamera(void){
	if (m_object) {
		gkScene* scene = cast<gkScene>();
		return scene->getMainCamera();
	}
	return 0;
}

void gsScene::setUpdateFlags(unsigned int flags)
{
	if (m_object)
	{
		cast<gkScene>()->setUpdateFlags(flags);
	}
}

void gsScene::setBackgroundColor(gsVector4 col)
{
	if (m_object)
	{
		cast<gkScene>()->getDisplayWindow()->getViewport(0)->getViewport()->setBackgroundColour(Ogre::ColourValue(col.x,col.y,col.z,col.w));
	}
}

gsVector2 gsScene::getScreenPos(const gsVector3& vec,bool absolute,bool outerFrustum)
{
	if (m_object)
	{
		return gsVector2(cast<gkScene>()->getScreenPos(vec,absolute,outerFrustum));
	}
	return gsVector2(-1.0f,-1.0f);
}


gkScene* getActiveScene(void)
{
	gkEngine* eng = gkEngine::getSingletonPtr();
	if (eng && eng->isInitialized())
		return eng->getActiveScene();
	return 0;
}

gkScene* addScene(const gkString& sceneName,int zorder) {
	if (!sceneName.empty())
	{
		gkScene* tmpScene = static_cast<gkScene*>(gkSceneManager::getSingleton().getByName(sceneName));
		if (!tmpScene->isInstanced())
		{
			gkWindow* win;
			win = gkEngine::getSingleton().getActiveScene()->getDisplayWindow();
			tmpScene->setDisplayWindow(win, zorder);
			tmpScene->createInstance(true);
		}
		return tmpScene;
	}
	return 0;
}

gkScene* addScene(gsScene* scene,int zorder) {
	if (scene)
	{
		gkScene* tmpScene = scene->cast<gkScene>();
		if (!tmpScene->isInstanced())
		{
		gkWindow* win;
		win = gkEngine::getSingleton().getActiveScene()->getDisplayWindow();
		tmpScene->setDisplayWindow(win, zorder);
		tmpScene->createInstance(true);
		}
		return tmpScene;
	}
	return 0;
}

gkScene* getScene(const gkString& sceneName, const gkString& groupName)
{
	return static_cast<gkScene*>(gkSceneManager::getSingleton().getByName(gkResourceName(sceneName,groupName)));
}

gkHUD* getHUD(const gkString& name)
{
	gkHUD* hud = gkHUDManager::getSingleton().getOrCreate(name);
	if (hud)
		return hud;
	return 0;
}

gsArray<gsGameObject, gkGameObject> &gsScene::getObjectList(void)
{
	m_objectCache.clear();


	if (m_object)
	{
		gkScene* scene = cast<gkScene>();
		gkGameObjectSet& objs = scene->getInstancedObjects();
		gkGameObjectSet::Iterator it = objs.iterator();

		while (it.hasMoreElements())
		{
			gkGameObject* obj = it.getNext();
			m_objectCache.push(obj);
		}
	}
	return m_objectCache;
}




gsGameObject::gsGameObject() : m_callback(0)
{
}



gsGameObject::gsGameObject(gkInstancedObject* ob) : gsObject(ob),m_callback(0)
{
}


void gsGameObject::addCallback(gsSelf self, gsFunction func){
	if (!m_callback){
		gkLuaEvent* event = new gkLuaEvent(self,func);
		gkGameObject* gobj = get();
		m_callback = new gsGameObjectCallback(gobj,event);
		gobj->addEventListener(m_callback);
	} else {
		gkDebugScreen::printTo("There is already a callback-function for "+getName());
	}
}

void gsGameObject::addCallback(gsFunction func){
	if (!m_callback){
		gkLuaEvent* event = new gkLuaEvent(func);
		gkGameObject* gobj = get();
		m_callback = new gsGameObjectCallback(gobj,event);
		gobj->addEventListener(m_callback);
	} else {
		gkDebugScreen::printTo("There is already a callback-function for "+getName());
	}
}

void gsGameObject::bakeTransform()
{
	gkGameObject* gobj = get();
	gobj->bakeTransformState();
}

void gsGameObject::removeCallback() {
	if (m_callback) {
		get()->removeEventListener(m_callback);
		delete m_callback;
		m_callback=0;
	}
}


gsVector2 gsGameObject::getScreenPos(bool absolute)
{
	return gsVector2(get()->getOwner()->getScreenPos(getWorldPosition(),absolute));
}

bool gsGameObject::isinfrustum(const gkVector3& coords)
{
   return ( ( coords.x >= -1 ) && ( coords.x <= 1 ) && ( coords.y >= -1 ) && ( coords.y <= 1 ) );
}

//check that target is in camera view
bool gsGameObject::isinfront(const gkVector3& worldview)
{
   return ( worldview.z < 0 );
}


gsVector3 gsGameObject::getPosition(void)
{
	if (m_object)
		return gsVector3(cast<gkGameObject>()->getPosition());
	return gsVector3();
}



gsVector3 gsGameObject::getRotation(void)
{
	if (m_object)
		return gsVector3(cast<gkGameObject>()->getRotation().toVector3());
	return gsVector3();
}



gsQuaternion gsGameObject::getOrientation(void)
{
	if (m_object)
		return gsQuaternion(cast<gkGameObject>()->getOrientation());
	return gsQuaternion();
}



gsVector3 gsGameObject::getScale(void)
{
	if (m_object)
		return gsVector3(cast<gkGameObject>()->getScale());
	return gsVector3();
}



gsVector3 gsGameObject::getWorldPosition(void)
{
	if (m_object)
		return gsVector3(cast<gkGameObject>()->getWorldPosition());
	return gsVector3();
}



gsVector3 gsGameObject::getWorldRotation(void)
{
	if (m_object)
		return gsVector3(cast<gkGameObject>()->getWorldRotation().toVector3());
	return gsVector3();
}

gsVector3 gsGameObject::getWorldScale(void)
{
	if (m_object)
		return gsVector3(cast<gkGameObject>()->getWorldScale());
	return gsVector3();
}

gsQuaternion gsGameObject::getWorldOrientation(void)
{
	if (m_object)
		return gsQuaternion(cast<gkGameObject>()->getWorldOrientation());
	return gsQuaternion();
}



gsVector3 gsGameObject::getLinearVelocity(void)
{
	if (m_object)
		return gsVector3(cast<gkGameObject>()->getLinearVelocity());
	return gsVector3();
}




gsVector3 gsGameObject::getAngularVelocity(void)
{
	if (m_object)
		return gsVector3(cast<gkGameObject>()->getAngularVelocity());
	return gsVector3();
}


void gsGameObject::applyForce(const gsVector3& v, gsTransformSpace ts)
{
	if (m_object)
		cast<gkGameObject>()->applyForce(v,ts);
}

void gsGameObject::applyForce(float x, float y, float z, gsTransformSpace ts)
{
	if (m_object)
		cast<gkGameObject>()->applyForce(gkVector3(x,y,z),ts);
}


void gsGameObject::setLinearVelocity(const gsVector3& v,gsTransformSpace space)
{
	if (m_object)
		cast<gkGameObject>()->setLinearVelocity(v,space);
}



void gsGameObject::setLinearVelocity(float x, float y, float z,gsTransformSpace space)
{
	if (m_object)
		cast<gkGameObject>()->setLinearVelocity(gkVector3(x, y, z),space);
}



void gsGameObject::setAngularVelocity(const gsVector3& v,gsTransformSpace space)
{
	if (m_object)
		cast<gkGameObject>()->setAngularVelocity(v,space);
}



void gsGameObject::setAngularVelocity(float x, float y, float z,gsTransformSpace space)
{
	if (m_object)
		cast<gkGameObject>()->setAngularVelocity(gkVector3(x, y, z),space);
}

void  gsGameObject::setGravity(const gsVector3& v)
{
	if (m_object)
		cast<gkGameObject>()->setGravity(v);
}

gsVector3 gsGameObject::getGravity(void)
{
	if (m_object)
		return cast<gkGameObject>()->getGravity();

	return gsVector3::ZERO;
}


gsVector3 gsGameObject::getLinearFactor(void)
{
	if (m_object) {
		gkRigidBody* rigidBody = cast<gkGameObject>()->getAttachedBody();

		if (rigidBody)
			return gsVector3(rigidBody->getLinearFactor());

		gkDebugScreen::printTo("Tried to get LinearFactor, but "+cast<gkGameObject>()->getName()+" but doens't have a rigidbody attached");
	}
	return gsVector3(0,0,0);
}
void gsGameObject::setLinearFactor(const gsVector3& v)
{
	if (m_object) {
		gkRigidBody* rigidBody = cast<gkGameObject>()->getAttachedBody();

		if (rigidBody)
			return rigidBody->setLinearFactor(v);

		gkDebugScreen::printTo("Tried to set LinearFactor, but "+cast<gkGameObject>()->getName()+" but doens't have a rigidbody attached");
	}
}
gsVector3 gsGameObject::getAngularFactor(void)
{
	if (m_object) {
		gkRigidBody* rigidBody = cast<gkGameObject>()->getAttachedBody();

		if (rigidBody)
			return gsVector3(rigidBody->getAngularFactor());

		gkDebugScreen::printTo("Tried to get AngularFactor, but "+cast<gkGameObject>()->getName()+" but doens't have a rigidbody attached");
	}
	return gsVector3(0,0,0);
}

void gsGameObject::setAngularFactor(const gsVector3& v)
{
	if (m_object) {
		gkRigidBody* rigidBody = cast<gkGameObject>()->getAttachedBody();

		if (rigidBody)
			return rigidBody->setAngularFactor(v);

		gkDebugScreen::printTo("Tried to set AngularFactor, but "+cast<gkGameObject>()->getName()+" but doens't have a rigidbody attached");
	}
}

void gsGameObject::clearForces(void)
{
	if (m_object) {
		gkRigidBody* rigidBody = cast<gkGameObject>()->getAttachedBody();

		if (rigidBody)
			return rigidBody->clearForces();

		gkDebugScreen::printTo("Tried to clear forces, but "+cast<gkGameObject>()->getName()+" but doens't have a rigidbody attached");
	}
}

void gsGameObject::setMass(gkScalar mass)
{
	if (m_object) {
		gkRigidBody* rigidBody = cast<gkGameObject>()->getAttachedBody();

		if (rigidBody)
			return rigidBody->setMass(mass);

		gkDebugScreen::printTo("Tried to set mass, but "+cast<gkGameObject>()->getName()+" but doens't have a rigidbody attached");
	}
}

void gsGameObject::setPosition(const gsVector3& v)
{
	if (m_object)
		cast<gkGameObject>()->setPosition(v);
}



void gsGameObject::setPosition(float x, float y, float z)
{
	if (m_object)
		cast<gkGameObject>()->setPosition(gkVector3(x, y, z));
}

void gsGameObject::lookAt(const gsVector3& v)
{
	gkVector3 front = gkVector3(v.x, v.y, v.z) - cast<gkGameObject>()->getWorldPosition();
	gkVector3 up    = gkVector3::UNIT_Z;
	gkVector3 right = front.crossProduct(up);
	up = right.crossProduct(front);
	front.normalise();
	up.normalise();
	right.normalise();
	cast<gkGameObject>()->setOrientation(gkQuaternion(right, up, -front));
}

gsQuaternion gsGameObject::fakeLookAt(const gsVector3& v)
{
	gkVector3 front = gkVector3(v.x, v.y, v.z) - cast<gkGameObject>()->getWorldPosition();
	gkVector3 up    = gkVector3::UNIT_Z;
	gkVector3 right = front.crossProduct(up);
	up = right.crossProduct(front);
	front.normalise();
	up.normalise();
	right.normalise();
	return gsQuaternion(gkQuaternion(right, up, -front));
}

void gsGameObject::setVisible(bool visible)
{
	if (m_object)
		cast<gkGameObject>()->setVisible(visible);
}

bool gsGameObject::isVisible()
{
	if (m_object)
		return cast<gkGameObject>()->isVisible();
	return false;
}

void gsGameObject::lookAt(gsGameObject* lookAtObj)
{
	if (m_object)
	{
		if (!lookAtObj)
		{
			gkDebugScreen::printTo("Warning: You passed NIL as lookAt-Object. Ignoring LookAt(..)!");
			return;
		}

		lookAt(lookAtObj->getPosition());
	}
}

void gsGameObject::setRotation(const gsVector3& v)
{
	if (m_object)
		cast<gkGameObject>()->setOrientation(gkEuler(v));
}



void gsGameObject::setRotation(float pitch, float yaw, float roll)
{
	if (m_object)
		cast<gkGameObject>()->setOrientation(gkEuler(pitch, yaw, roll));
}



void gsGameObject::setOrientation(const gsQuaternion& quat)
{
	if (m_object)
		cast<gkGameObject>()->setOrientation(quat);
}



void gsGameObject::setOrientation(float w, float x, float y, float z)
{
	if (m_object)
		cast<gkGameObject>()->setOrientation(gkQuaternion(w, x, y, z));
}

void gsGameObject::setScale(const gsVector3& v)
{
	if (m_object)
	{
		get()->setScale(gkVector3(v.x,v.y,v.z));
	}
}

void gsGameObject::setScale(float x,float y,float z)
{
	if (m_object)
	{
		get()->setScale(gkVector3(x,y,z));
	}
}

gsGameObjectTypes gsGameObject::getType(void)
{
	if (m_object)
		return (gsGameObjectTypes)cast<gkGameObject>()->getType();
	return OBT_UNKNOWN;
}


void gsGameObject::tweenToPos(const gsVector3& to,float ptime,TweenTransition ptransition, TweenEq pequation, float pdelay)
{
	if (m_object)
		cast<gkGameObject>()->tweenToPos(to,ptime,ptransition,pequation,pdelay);
}


void gsGameObject::rotate(float dx, float dy, float dz)
{
	if (m_object)
		cast<gkGameObject>()->rotate(gkEuler(dx, dy, dz), TRANSFORM_PARENT);
}


void gsGameObject::rotate(const gsVector3& v)
{
	if (m_object)
		cast<gkGameObject>()->rotate(gkEuler(v), TRANSFORM_PARENT);
}



void gsGameObject::rotate(const gsQuaternion& v)
{
	if (m_object)
		cast<gkGameObject>()->rotate(v, TRANSFORM_PARENT);
}



void gsGameObject::rotate(float dx, float dy, float dz, gsTransformSpace ts)
{
	if (m_object)
		cast<gkGameObject>()->rotate(gkEuler(dx, dy, dz), ts);
}



void gsGameObject::rotate(const gsVector3& v, gsTransformSpace ts)
{
	if (m_object)
		cast<gkGameObject>()->rotate(gkEuler(v), ts);
}



void gsGameObject::rotate(const gsQuaternion& v, gsTransformSpace ts)
{
	if (m_object)
		cast<gkGameObject>()->rotate(v, ts);
}



void gsGameObject::translate(const gsVector3& v)
{
	if (m_object)
		cast<gkGameObject>()->translate(v, TRANSFORM_PARENT);
}



void gsGameObject::translate(float x, float y, float z)
{
	if (m_object)
		cast<gkGameObject>()->translate(gkVector3(x, y, z), TRANSFORM_PARENT);
}


void gsGameObject::translate(const gsVector3& v, gsTransformSpace ts)
{
	if (m_object)
		cast<gkGameObject>()->translate(v, ts);
}


void gsGameObject::translate(float x, float y, float z, gsTransformSpace ts)
{
	if (m_object)
		cast<gkGameObject>()->translate(gkVector3(x, y, z), ts);
}


void gsGameObject::scale(const gsVector3& v)
{
	if (m_object)
		cast<gkGameObject>()->scale(v);
}



void gsGameObject::scale(float x, float y, float z)
{
	if (m_object)
		cast<gkGameObject>()->scale(gkVector3(x, y, z));
}

gsVector3 gsGameObject::getDimension()
{
	gkGameObject* gobj = cast<gkGameObject>();

	return gobj->getType() == GK_ENTITY ?
			gsVector3(gobj->getEntity()->getMesh()->getDimension())
			:gsVector3(0,0,0);
}

void gsGameObject::yaw(float deg)
{
	if (m_object)
		cast<gkGameObject>()->yaw(gkDegree(deg), TRANSFORM_PARENT);
}




void gsGameObject::yaw(float deg, gsTransformSpace ts)
{
	if (m_object)
		cast<gkGameObject>()->yaw(gkRadian(deg), ts);
}



void gsGameObject::pitch(float deg)
{
	if (m_object)
		cast<gkGameObject>()->pitch(gkRadian(deg), TRANSFORM_PARENT);
}



void gsGameObject::pitch(float deg, gsTransformSpace ts)
{
	if (m_object)
		cast<gkGameObject>()->pitch(gkRadian(deg), ts);
}



void gsGameObject::roll(float deg)
{
	if (m_object)
		cast<gkGameObject>()->roll(gkRadian(deg), TRANSFORM_PARENT);
}



void gsGameObject::roll(float deg, gsTransformSpace ts)
{
	if (m_object)
		cast<gkGameObject>()->roll(gkRadian(deg), ts);
}

//
//class ContactCallback : public btCollisionWorld::ContactResultCallback
//{
//public:
//	ContactCallback() :
//
//};
//
//void gsGameObject::explicitContactTest(gsFunction func)
//{
//	if (m_object)
//	{
//		gkGameObject* gobj = cast<gkGameObject>();
//
//		btCollisionObject* btObj = gobj->getCollisionObject();
//		if (btObj) {
//			gobj->getOwner()->getDynamicsWorld()->getBulletWorld()->contactTest(btObj);
//		}
//	}
//}


gkVariable* gsGameObject::getPropertyRaw(const gkString& name)
{
	if (m_object)
	{
		gkVariable* var = cast<gkGameObject>()->getVariable(name);
		if (var)
			return var;
	}
	return 0;
}

bool gsGameObject::hasProperty(const gkString& name)
{
	if (m_object)
	{
		return cast<gkGameObject>()->hasVariable(name);
	}
	return false;
}


gsProperty gsGameObject::__getitem__(const gkString& prop)
{
	if (m_object)
	{
		gkVariable* var = cast<gkGameObject>()->getVariable(prop);
		if (var)
			return gsProperty(var);
	}
	return gsProperty();
}



void gsGameObject::__setitem__(const gkString& prop, bool  v)
{
	if (m_object)
	{
		gkVariable* var = cast<gkGameObject>()->getVariable(prop);

		if (!var)
			var = cast<gkGameObject>()->createVariable(prop,false);

		(*var).setValue(v);
	}
}

void gsGameObject::__setitem__(const gkString& prop, gsVector3  v)
{
	if (m_object)
	{
		gkVariable* var = cast<gkGameObject>()->getVariable(prop);

		if (!var)
			var = cast<gkGameObject>()->createVariable(prop,false);

		(*var).setValue(v);
	}
}


void gsGameObject::__setitem__(const gkString& prop, float v)
{
	if (m_object)
	{
		gkVariable* var = cast<gkGameObject>()->getVariable(prop);
		if (!var)
			var = cast<gkGameObject>()->createVariable(prop,false);

		(*var).setValue(v);
	}
}



void gsGameObject::__setitem__(const gkString& prop, const char* v)
{
	if (m_object)
	{
		gkVariable* var = cast<gkGameObject>()->getVariable(prop);
		if (!var)
			var = cast<gkGameObject>()->createVariable(prop,false);

		(*var).setValue(gkString(v));
	}
}



gkScene* gsGameObject::getScene(void)
{
	if (m_object)
		return cast<gkGameObject>()->getOwner();
	return 0;
}



int gsGameObject::getState(void)
{
	if (m_object)
		return cast<gkGameObject>()->getState();
	return -1;
}

void gsGameObject::changeState(int v)
{
	if (m_object)
		cast<gkGameObject>()->changeState(v);
}

bool gsGameObject::hasParent()
{
	return m_object && cast<gkGameObject>()->getParent() != 0;
}



void gsGameObject::setParent(gsGameObject* par)
{
	if (m_object && par)
	{
		gkGameObject* gobj = get();
		gkGameObject* pobj = par->get();

		if (gobj == pobj) return;

		if (!gobj->hasParent())
			gobj->setParent(pobj);
		else
		{
			if (gobj->getParent() != pobj)
			{
				gobj->getParent()->removeChild(gobj);
				gobj->setParent(pobj);
			}
		}
	}
}



void gsGameObject::setParentInPlace(gsGameObject* par)
{
	if (m_object && par)
	{
		gkGameObject* gobj = get();
		gkGameObject* pobj = par->get();

		if (gobj == pobj) return;

		if (!gobj->hasParent())
			gobj->setParentInPlace(pobj);
		else
		{
			if (gobj->getParent() != pobj)
			{
				gobj->clearParentInPlace();
				gobj->setParentInPlace(pobj);
			}
		}
	}
}



void gsGameObject::clearParent(void)
{
	if (m_object)
	{
		gkGameObject* gobj = get();

		if (gobj->hasParent())
			gobj->clearParent();
	}
}



void gsGameObject::clearParentInPlace(void)
{
	if (m_object)
	{
		gkGameObject* gobj = get();

		if (gobj->hasParent())
			gobj->clearParentInPlace();
	}
}



void gsGameObject::addChild(gsGameObject* chi)
{
	if (m_object && chi)
	{
		gkGameObject* gobj = get();
		gkGameObject* cobj = chi->get();
		if (gobj == cobj) return;

		if (!gobj->hasChild(cobj))
			gobj->addChild(cobj);
	}
}


void gsGameObject::removeChild(gsGameObject* chi)
{
	if (m_object && chi)
	{
		gkGameObject* gobj = get();
		gkGameObject* cobj = chi->get();
		if (gobj == cobj) return;


		if (gobj->hasChild(cobj))
			gobj->removeChild(cobj);
	}
}



gkGameObject* gsGameObject::getParent(void)
{
	return m_object && hasParent() ?
	       cast<gkGameObject>()->getParent() : 0;
}



void gsGameObject::enableContacts(bool v)
{
	if (m_object)
	{
		gkPhysicsController* ob = get()->getPhysicsController();
		if (ob)
			ob->enableContactProcessing(v);
	}
}

gsContactInfo gsGameObject::getContactInfo(int nr)
{
	if (m_object)
	{
		gkPhysicsController* ob = get()->getPhysicsController();
		if (ob)
		{
			gkContactInfo info = ob->getContacts()[nr];

			return gsContactInfo(info.point);
		}
	}
	return gsContactInfo();
}

int gsGameObject::getContactCount()
{
	if (m_object)
	{
		gkPhysicsController* ob = get()->getPhysicsController();
		if (ob)
		{
			return ob->getContacts().size();
		}
	}
	return 0;
}

gkGameObject* gsGameObject::getContact(int nr)
{
	if (m_object)
	{
		gkPhysicsController* ob = get()->getPhysicsController();
		if (ob)
		{
			if (ob->getContacts().size() > nr)
				return ob->getContacts()[nr].collider->getObject();
		}
	}
	return 0;
}

bool gsGameObject::hasContacts()
{
	if (m_object)
	{
		gkPhysicsController* ob = get()->getPhysicsController();
		if (ob)
			return !ob->getContacts().empty();
	}
	return false;
}



bool gsGameObject::hasContact(const gkString& object)
{
	if (m_object)
	{
		gkPhysicsController* ob = get()->getPhysicsController();
		if (ob)
			return ob->collidesWith(object);
	}
	return false;
}


bool gsGameObject::isVehicle()
{
	return get()->getAttachedVehicle() != 0;
}
gkVehicle* gsGameObject::getVehicle()
{
	return get()->getAttachedVehicle();
}

gkLogicTree* gsGameObject::getLogicTree()
{
	return get()->getLogic();
}


gkAnimationPlayer* gsGameObject::getAnimation(const gkString& name, bool loop)
{
	if (m_object && m_object->isInstanced())
	{
		gkAnimationPlayer* player = get()->getAnimationPlayer(name);
		if (player == 0)
		{
			player = get()->addAnimation(name);
			if (!player)
			{
				gsDebugPrint(gkString("Couldn't find animation with name:"+name).c_str());
				return 0;
			}
		}

		player->setMode(loop?AK_ACT_LOOP:AK_ACT_END);

		return player;
	}

	return 0;
}


int gsGameObject::getAnimationCount()
{
	return get()->getAnimations().size();
}

gkString gsGameObject::getAnimationName(int nr)
{
	if (nr < get()->getAnimations().size())
	{
		return get()->getAnimations().keyAt(nr).str();
	}
	return "";
}

gkAnimationPlayer* gsGameObject::playAnimation(const gkString& name, float blend, bool restart, bool loop)
{
	if (m_object && m_object->isInstanced())
	{
		gkAnimationPlayer* player = get()->getAnimationPlayer(name);
		if (player == 0)
		{
			player = get()->addAnimation(name);
			if (!player)
			{
				gsDebugPrint(gkString("Couldn't find animation with name:"+name).c_str());
				return 0;
			}
		}
		if(restart){
			player->reset();
		}

		player->setMode(loop?AK_ACT_LOOP:AK_ACT_END);

		get()->playAnimation(name, blend, restart);

		return player;
	}

	return 0;
}

void gsGameObject::playAnimation(gsAnimationPlayer* player,float blend, bool restart)
{
	if (m_object && m_object->isInstanced() && player)
	{
		if(restart){
			player->reset();
		}
		get()->playAnimation(player->get(), blend, player->getMode());
	}

}

void gsGameObject::stopAnimation(const gkString& name)
{
	get()->stopAnimation(name);
}

void gsGameObject::pauseAnimations()
{
	get()->pauseAnimations();
}
void gsGameObject::resumeAnimations()
{
	get()->resumeAnimations();
}



gkGameObject* gsGameObject::getChildAt(int pos)
{
	if (m_object)
	{
		if (pos < (int)get()->getChildren().size())
			return get()->getChildren().at(pos);
		else
		{
			gkString error = gkString("Index out of bounds ")+gkToString(pos)+" in "+get()->getName();
			gsDebugPrint(error.c_str());
			return 0;
		}
	}
	return 0;
}

void gsGameObject::suspendPhysics(bool suspend)
{
	if (m_object)
	{
		gkPhysicsController* phys = get()->getPhysicsController();
		if (phys)
		{

			phys->suspend(suspend);
		}
	}
}

int gsGameObject::getChildCount(void)
{
	if (m_object)
	{
		return get()->getChildren().size();
	}
	return 0;
}

gkGameObject* gsGameObject::getChildByName(const gkString& name)
{
	if (m_object)
	{
		gkGameObjectArray::Iterator it = get()->getChildren().iterator();
		while (it.hasMoreElements())
		{
			gkGameObject* gobj = it.getNext();
			if (gobj->getName() == name)
				return gobj;
		}
	}
	return 0;
}

gkGameObjectInstance* gsGameObject::getGroupInstance()
{
	if (m_object)
	{
		gkGameObjectInstance* ginst = get()->getGroupInstance();
		return ginst;
	}
	return 0;
}


bool  gsGameObject::isGroupInstance()
{
	if (m_object)
	{
		return get()->isGroupInstance();
	}
	return false;
}

void gsGameObject::setTransform(const gsMatrix4& vmat)
{
	if (m_object)
	{
		get()->setTransform(vmat);
	}
}

gsEntity::gsEntity()
	: mAnimState(0)
{
}


gsEntity::gsEntity(gkInstancedObject* ob) : gsGameObject(ob),mAnimState(0)
{
}

void gsEntity::setOgreMeshName(const gkString& meshName)
{
	gkEntity* ent = static_cast<gkEntity*>(get());
	ent->setExternalMeshName(meshName);

//	if (ent->isInstanced())
//	{
//		ent->reinstance(true);
//	}
}

void gsEntity::playOgreAnimation(const gkString& animationName,bool loop)
{
	if (mAnimState!=0)
	{
		mAnimState->setEnabled(false);
	}
	gkEntity* ent = static_cast<gkEntity*>(get());
	mAnimState = ent->getEntity()->getAnimationState(animationName);

//	Ogre::AnimationStateSet* animSet = ent->getEntity()->getAllAnimationStates();

//	Ogre::AnimationStateIterator animIter(animSet->getAnimationStateIterator());
//	while (animIter.hasMoreElements())
//	{
//		mAnimState = animIter.getNext();
//		gkLogger::write("AnimState:"+mAnimState->getAnimationName(),true);
//
//	}

	if (mAnimState == 0)
	{
		gkLogger::write("Unknown ogre-animation "+animationName);
		return;
	}
	mAnimState->setLoop(loop);
	mAnimState->setEnabled(true);
}


void gsEntity::updateOgreAnimation(float dt)
{
	if (mAnimState)
	{
		mAnimState->addTime(dt);
	}
}


bool gsEntity::hasHardwareSkinning()
{
	gkEntity* ent = static_cast<gkEntity*>(get());
	return ent->getEntity()->isHardwareAnimationEnabled();

}

gkMesh* gsEntity::getMesh() {
	gkEntity* ent = static_cast<gkEntity*>(get());
	return ent->getMesh();
}
bool gsEntity::hasCharacter(void)
{
	return get()->getAttachedCharacter() != 0;
}

gkCharacter* gsEntity::getCharacter(void)
{
	if (!hasCharacter()) return 0;

	return get()->getAttachedCharacter();
}

void gsEntity::setMaterialName(const gkString& name) {
	static_cast<gkEntity*>(m_object)->setMaterialName(name);
}

void gsSkeleton::attachObjectToBone(const gkString& boneName, gsGameObject* gsobj,gsVector3 loc,gsVector3 orientation,gsVector3 scale)
{
	if (m_object)
	{
		gkSkeleton* skel = static_cast<gkSkeleton*>(m_object);
		gkTransformState* trans = new gkTransformState(loc,gkEuler(orientation).toQuaternion(),scale);
		skel->attachObjectToBone(boneName,gsobj->cast<gkGameObject>(),trans);
	}
}

void gsSkeleton::attachObjectToBoneInPlace(const gkString& boneName, gsGameObject* gsobj)
{
	if (m_object)
	{
		gkSkeleton* skel = static_cast<gkSkeleton*>(m_object);
		skel->attachObjectToBoneInPlace(boneName,gsobj->cast<gkGameObject>());
	}
}


gsVector3 gsSkeleton::getBonePosition(const gkString& boneName)
{
	if (m_object)
	{
		gkSkeleton* skel = static_cast<gkSkeleton*>(m_object);
		gkBone* bone = skel->getBone(boneName);
		if (bone)
		{
			gkVector3 trans = bone->getTransform().getTrans();
			return gsVector3(trans);
		}
		else gsDebugPrint(gkString("Skeleton doesn't have a bone with name "+boneName).c_str());
	}
	return gsVector3(0,0,0);
}

void gsSkeleton::setBoneManual(const gkString& boneName,bool setManual)
{
	if (m_object)
	{
		gkSkeleton* skel = static_cast<gkSkeleton*>(m_object);
		gkBone* bone = skel->getBone(boneName);
		if (bone)
		{
			bone->setManuallyControlled(setManual);
		}
		else gsDebugPrint(gkString("Skeleton doesn't have a bone with name "+boneName).c_str());
	}
}

bool gsSkeleton::isBoneManual(const gkString& boneName)
{
	if (m_object)
	{
		gkSkeleton* skel = static_cast<gkSkeleton*>(m_object);

		gkBone* bone = skel->getBone(boneName);
		if (bone)
		{
			return bone->isManuallyControlled();
		}  // if
		else gsDebugPrint(gkString("Skeleton does have a bone with name "+boneName).c_str());
	}  // if
	return false;
}

void gsSkeleton::applyBoneChannelTransform(const gkString& boneName, gsVector3 loc, gsVector3 orientation,gsVector3 scale, gkScalar pWeight)
{
	if (m_object)
	{
		gkSkeleton* skel = static_cast<gkSkeleton*>(m_object);

		gkBone* bone = skel->getBone(boneName);
		if (bone)
		{
			gkTransformState lState = gkTransformState(loc,gkEuler(orientation).toQuaternion(),scale);
			bone->applyChannelTransform(lState, pWeight);
		}  // if
		else gsDebugPrint(gkString("Skeleton does have a bone with name "+boneName).c_str());
	}  // if
}  // void applyBoneChannelTransform

gsLight::gsLight()
{
}



gsLight::gsLight(gkInstancedObject* ob) : gsGameObject(ob)
{
}


gsCurve::gsCurve()
{
}

gsCurve::gsCurve(gkInstancedObject* ob) : gsGameObject(ob)
{
}

int gsCurve::getPointCount(int subcurve)
{
	if (m_object){
		return static_cast<gkCurve*>(m_object)->getPointCount(subcurve);
	}
	return -1;
}

void gsCurve::generateBezierPoints(float nr) {
	if (m_object){
			static_cast<gkCurve*>(m_object)->generateBezierPoints(nr);
	}
}

int gsCurve::getSubCurveAmount()
{
	if (m_object){
		return static_cast<gkCurve*>(m_object)->getSubCurveAmount();
	}
	return 0;
}

bool gsCurve::isCyclic(int subcurve)
{
	if (m_object){
		return static_cast<gkCurve*>(m_object)->isCyclic(subcurve);
	}
	return false;
}

bool gsCurve::isBezier(int subcurve)
{
	if (m_object) {
		return static_cast<gkCurve*>(m_object)->isBezier(subcurve);
	}
	return false;
}

gsVector3 gsCurve::getPoint(int nr,int subcurve)
{
	if (m_object){
		return gsVector3(static_cast<gkCurve*>(m_object)->getPoint(nr,subcurve));
	}
	return gsVector3(0,0,0);
}

gsVector3 gsCurve::getDeltaPoint(float dt,int subcurve)
{
	if (m_object)
		return gsVector3(static_cast<gkCurve*>(m_object)->getDeltaPoint(dt,subcurve));
	return gsVector3(0,0,0);
}



void  gsCamera::setClipping(float start, float end)
{
	if (m_object)
		cast<gkCamera>()->setClip(gkMax(start, 0.0001f), end);
}



float gsCamera::getClipStart()
{
	if (m_object)
		return cast<gkCamera>()->getCameraProperties().m_clipstart;
	return 0;
}

bool gsCamera::isOrtho()
{
	if (m_object)
			return cast<gkCamera>()->getProjType() == gkCameraProperties::CA_ORTHOGRAPHIC;
	return false;
}
void gsCamera::setOrthoScale(float scale)
{
	if (m_object)
			cast<gkCamera>()->setOrthoScale(scale);
}
float gsCamera::getOrthoScale()
{
	if (m_object)
			return cast<gkCamera>()->getOrthoScale();
	return 0;
}

gsCamera::gsCamera()
{
}


gsCamera::gsCamera(gkInstancedObject* ob) : gsGameObject(ob)
{
}


float gsCamera::getClipEnd()
{
	if (m_object)
		return cast<gkCamera>()->getCameraProperties().m_clipend;
	return 0;
}



void  gsCamera::setFov(float fov)
{
	if (m_object)
		cast<gkCamera>()->setFov(gkDegree(fov));
}



float gsCamera::getFov()
{
	if (m_object)
		return cast<gkCamera>()->getCameraProperties().m_fov;
	return 0;
}



void gsCamera::makeCurrent()
{
	if (m_object)
		cast<gkCamera>()->makeCurrent();
}

void gsCamera::setProjection(const gsMatrix4& mat4)
{
	cast<gkCamera>()->getCamera()->setCustomProjectionMatrix(true,mat4);
}

gsSkeleton::gsSkeleton()
{
}



gsSkeleton::gsSkeleton(gkInstancedObject* ob) : gsGameObject(ob)
{
}


gsParticles::gsParticles()
{
}



gsParticles::gsParticles(gkInstancedObject* ob) : gsGameObject(ob)
{
}


gsDebugger::gsDebugger(gkDebugger* db)
	: m_debugger(db)
{}

gsDebugger::gsDebugger(gsScene* sc)
	:   m_debugger(0)
{
	if (sc)
	{
		gkScene* scene = sc->cast<gkScene>();
		if (scene)
			m_debugger = scene->getDebugger();

	}
}

void gsDebugger::drawPath(gsNavPath* path,const gsVector3& color)
{
	m_debugger->drawPath(path->get());
}


void gsDebugger::drawLine(const gsVector3& from, const gsVector3& to, const gsVector3& color)
{
	if (m_debugger)
		m_debugger->drawLine(from, to, color);
}

void gsDebugger::drawCircle(const gsVector3& center,float radius,const gsVector3& color)
{
	if (m_debugger)
		m_debugger->drawCircle(center,radius,color);
}


void gsDebugger::drawObjectAxis(gsGameObject* ptr, float size)
{
	if (m_debugger && ptr)
	{
		gkGameObject* ob = ptr->cast<gkGameObject>();
		if (ob)
		{
			const gkVector3&     axis   = ob->getWorldPosition();
			const gkQuaternion&  ori    = ob->getOrientation();

			gkVector3 x = (ori * gkVector3(size, 0, 0));
			gkVector3 y = (ori * gkVector3(0, size, 0));
			gkVector3 z = (ori * gkVector3(0, 0, size));


			m_debugger->drawLine(axis, axis + x, gkVector3(1, 0, 0));
			m_debugger->drawLine(axis, axis + y, gkVector3(0, 1, 0));
			m_debugger->drawLine(axis, axis + z, gkVector3(0, 0, 1));
		}
	}
}

void gsDebugger::drawCurve(gsCurve* curve, const gsVector3& color)
{
	if (m_debugger && curve)
		m_debugger->drawCurve(curve->cast<gkCurve>(), gkVector3(color));
}


void gsDebugger::clear(void)
{
	if (m_debugger)
		m_debugger->clear();
}

void gsRecastDebugger::drawPath(gsNavPath* path,const gsVector4& col){
	m_recastDebugger->drawPath(path->get(),gkColor(col.x,col.y,col.z,col.w));
}



void gsDebugPrint(const char* str)
{
	gkDebugScreen::printTo(str);
}

void sendMessage(const gkString& from,const gkString& to,const gkString& subject,const char* body){
	int len = strlen(body);
	gkString str(body);
	int l2 = str.length();
	gkMessageManager::getSingletonPtr()->sendMessage(from,to,subject,body);
}

bool gsSetCompositorChain(gsCompositorOp op, const gkString& compositorName)
{
#ifdef OGREKIT_USE_COMPOSITOR
	return gkCompositorManager::getSingleton().setCompositorChain((gkCompositorOp)op, compositorName);
#else
	return false;
#endif
}

void setMaterialParam(const gkString& matName, int shaderType,const gkString& paramName, int value)
{
	Ogre::MaterialPtr matPtr = Ogre::MaterialManager::getSingleton().getByName(matName).staticCast<Ogre::Material>();
	if (!matPtr.isNull())
	{
		Ogre::Material::TechniqueIterator iter(matPtr.get()->getTechniqueIterator());
		while (iter.hasMoreElements())
		{
			Ogre::Technique* technique = iter.getNext();
			Ogre::Technique::PassIterator passIter(technique->getPassIterator());
			while (passIter.hasMoreElements())
			{
				Ogre::Pass* pass = passIter.getNext();
				if (shaderType == ST_VERTEX)
				{
					Ogre::GpuProgramParametersSharedPtr params = pass->getVertexProgramParameters();
					if (!params.isNull()){
						params.get()->setNamedConstant(paramName,value);
					}
				}
				else if (shaderType == ST_FRAGMENT)
				{
					Ogre::GpuProgramParametersSharedPtr params = pass->getFragmentProgramParameters();
					if (!params.isNull() && params.get()->_findNamedConstantDefinition(paramName,false)){
						params.get()->setNamedConstant(paramName,value);
					}
				}



			}
		}
	} else {
		gkDebugScreen::printTo("Unknown Material:"+matName);
	}
}

void setMaterialParam(const gkString& matName, int shaderType,const gkString& paramName, float value)
{
	Ogre::MaterialPtr matPtr = Ogre::MaterialManager::getSingleton().getByName(matName).staticCast<Ogre::Material>();
	if (!matPtr.isNull())
	{
		Ogre::Material::TechniqueIterator iter(matPtr.get()->getTechniqueIterator());
		while (iter.hasMoreElements())
		{
			Ogre::Technique* technique = iter.getNext();
			Ogre::Technique::PassIterator passIter(technique->getPassIterator());
			while (passIter.hasMoreElements())
			{
				Ogre::Pass* pass = passIter.getNext();
				if (shaderType == ST_VERTEX)
				{
					Ogre::GpuProgramParametersSharedPtr params = pass->getVertexProgramParameters();
					if (!params.isNull()){
						params.get()->setNamedConstant(paramName,value);
					}
				}
				else if (shaderType == ST_FRAGMENT)
				{
					Ogre::GpuProgramParametersSharedPtr params = pass->getFragmentProgramParameters();
					if (!params.isNull() && params.get()->_findNamedConstantDefinition(paramName,false)){
						params.get()->setNamedConstant(paramName,value);
					}
				}



			}
		}
	} else {
		gkDebugScreen::printTo("Unknown Material:"+matName);
	}
}


void setMaterialParam(const gkString& matName, int shaderType,const gkString& paramName, gsVector3* value)
{
	Ogre::MaterialPtr matPtr = Ogre::MaterialManager::getSingleton().getByName(matName).staticCast<Ogre::Material>();
	if (!matPtr.isNull())
	{
		Ogre::Material::TechniqueIterator iter(matPtr.get()->getTechniqueIterator());
		while (iter.hasMoreElements())
		{
			Ogre::Technique* technique = iter.getNext();
			Ogre::Technique::PassIterator passIter(technique->getPassIterator());
			while (passIter.hasMoreElements())
			{
				Ogre::Pass* pass = passIter.getNext();
				if (shaderType == ST_VERTEX)
				{
					Ogre::GpuProgramParametersSharedPtr params = pass->getVertexProgramParameters();
					if (!params.isNull()){
						params.get()->setNamedConstant(paramName,gkVector3(*value));
					}
				}
				else if (shaderType == ST_FRAGMENT)
				{
					Ogre::GpuProgramParametersSharedPtr params = pass->getFragmentProgramParameters();
					if (!params.isNull() && params.get()->_findNamedConstantDefinition(paramName,false)){
						params.get()->setNamedConstant(paramName,gkVector3(*value));
					}
				}



			}
		}
	} else {
		gkDebugScreen::printTo("Unknown Material:"+matName);
	}
}

void setMaterialParam(const gkString& matName, int shaderType,const gkString& paramName, gsVector4* value)
{
	Ogre::MaterialPtr matPtr = Ogre::MaterialManager::getSingleton().getByName(matName).staticCast<Ogre::Material>();
	if (!matPtr.isNull())
	{
		Ogre::Material::TechniqueIterator iter(matPtr.get()->getTechniqueIterator());
		while (iter.hasMoreElements())
		{
			Ogre::Technique* technique = iter.getNext();
			Ogre::Technique::PassIterator passIter(technique->getPassIterator());
			while (passIter.hasMoreElements())
			{
				Ogre::Pass* pass = passIter.getNext();
				if (shaderType == ST_VERTEX)
				{
					Ogre::GpuProgramParametersSharedPtr params = pass->getVertexProgramParameters();
					if (!params.isNull()){
						params.get()->setNamedConstant(paramName,gkVector4(value->x,value->y,value->z,value->w));
					}
				}
				else if (shaderType == ST_FRAGMENT)
				{
					Ogre::GpuProgramParametersSharedPtr params = pass->getFragmentProgramParameters();
					if (!params.isNull() && params.get()->_findNamedConstantDefinition(paramName,false)){
						params.get()->setNamedConstant(paramName,gkVector4(value->x,value->y,value->z,value->w));
					}
				}



			}
		}
	} else {
		gkDebugScreen::printTo("Unknown Material:"+matName);
	}
}


gkGameObjectInstance* createGroupInstance(const gkString& groupName,gsVector3 loc,gsVector3 rot,gsVector3 scale,const gkString& resGroup){
	gkEngine* eng = gkEngine::getSingletonPtr();
	if (eng && eng->isInitialized()){
		gkScene* scene = eng->getActiveScene();
		return createGroupInstance(static_cast<gkScene*>(scene),groupName,loc,rot,scale,resGroup);
	}
	return 0;
}

gkGameObjectInstance* createGroupInstance(gsScene* gScene,const gkString& groupName,gsVector3 loc,gsVector3 rot,gsVector3 scale,const gkString& resGroup){
	if (gScene)
	{
		gkScene* scene = gScene->getOwner();
		if (scene)
			return createGroupInstance(scene,groupName,loc,rot,scale,resGroup);
	}
	return 0;
}

gkGameObjectInstance* createGroupInstance(gsScene* gScene,const gkString& name,const gkString& resGroup)
{
	return createGroupInstance(gScene,name,gsVector3(0,0,0),gsVector3(0,0,0),gsVector3(1,1,1),resGroup);
}


gkGameObjectInstance* createGroupInstance(gkScene* scene,const gkString& groupName,gsVector3 loc,gsVector3 rot,gsVector3 scale,const gkString& resGroup){
	gkGroupManager* mgr = gkGroupManager::getSingletonPtr();
	if (mgr->exists(groupName))
	{
		gkGameObjectGroup* ggobj = (gkGameObjectGroup*)mgr->getByName(groupName);


		gkEngine* eng = gkEngine::getSingletonPtr();
		if (eng && eng->isInitialized()){
			if (!scene)
				scene = eng->getActiveScene();
			gkGameObjectInstance* inst = ggobj->createGroupInstance(scene, gkResourceName(gkUtils::getUniqueName("gi"+groupName), resGroup), 0,scene->getLayer());
			gkQuaternion quat;
			quat = gkEuler(gkVector3(rot.x,rot.y,rot.z)).toQuaternion();
			gkVector3 pos(loc.x,loc.y,loc.z);
			gkVector3 scl(scale.x,scale.y,scale.z);
			inst->getRoot()->getProperties().m_transform =  gkTransformState(pos, quat, scl);
			bool b = inst->isInstanced();
			if (!b){
				inst->createInstance(false);
			}

			return inst;
		}
	}
	return NULL;
}

void destroyResourceGroup(const gkString& resGrpName)
{
	gkResourceGroupManager::getSingleton().destroyResourceGroup(resGrpName);
}

void updateStaticBatch(gkString groupName,gsScene* gsscene)
{
	gkScene* scene = gsscene?gsscene->getOwner():gkEngine::getSingleton().getActiveScene();

	gkGroupManager* mgr = gkGroupManager::getSingletonPtr();

	if (groupName.empty())
	{
		// update all groups of the scene
		mgr->createStaticBatches(scene);
	}
	else // update a specific group
	{
		if (mgr->exists(groupName))
		{
			gkGameObjectGroup* ggobj = (gkGameObjectGroup*)mgr->getByName(groupName);
			ggobj->createStaticBatches(scene);
		}
	}
}

bool groupExists(gkString name)
{
	return gkGroupManager::getSingleton().exists(name);
}

int getGroupCount(gsScene* gsscene)
{
	gkScene* scene = gsscene?gsscene->getOwner():gkEngine::getSingleton().getActiveScene();

	return gkGroupManager::getSingleton().getSceneGroups(scene).size();
}

gkString getGroupNameAt(int i,gsScene* gsscene)
{
	gkScene* scene = gsscene?gsscene->getOwner():gkEngine::getSingleton().getActiveScene();

	gkGroupManager::Groups groups = gkGroupManager::getSingleton().getSceneGroups(scene);

	if (i < groups.size())
		return groups.at(i)->getName();
	else
		return "outofbound:"+gkToString((int)i)+"<"+gkToString((int)groups.size());
}

gsGameObjectInstance::gsGameObjectInstance( gkGameObjectInstance* inst ) : m_gobj(inst)
{
}

gsGameObjectInstance::~gsGameObjectInstance() {
}

void gsGameObjectInstance::destroyInstance()
{
	if (m_gobj)
		m_gobj->destroyInstance(false);
	else
		gkDebugScreen::printTo("Tried to destory invalid group-instance!?! Maybe you destroyed it already?");
}

void gsGameObjectInstance::createInstance()
{
	if (m_gobj)
		m_gobj->createInstance(false);
}

bool gsGameObjectInstance::isInstanced()
{
	if (m_gobj)
		return m_gobj->isInstanced();
	return false;
}

void gsGameObjectInstance::reinstance()
{
	if (m_gobj)
		m_gobj->reinstance(false);
}

void gsGameObjectInstance::kill(void)
{
	if (m_gobj)
	{
		m_gobj->getGroup()->destroyGroupInstance(m_gobj);
		m_gobj = 0;
	}
}

gkString gsGameObjectInstance::getGroupName()
{
	if (m_gobj)
	{
		return m_gobj->getGroup()->getName();
	}
	return "";
}


int gsGameObjectInstance::getElementCount()
{
	return m_gobj?m_gobj->getObjects().size():0;
}

gkGameObject* gsGameObjectInstance::getElementAt(int pos)
{
	return m_gobj?m_gobj->getObjects().at(pos):NULL;
}

gkGameObject* gsGameObjectInstance::getElementByName(gkString name)
{
	return m_gobj?m_gobj->getObject(name):NULL;
}

gkGameObject* gsGameObjectInstance::getRoot(void)
{
	return m_gobj->getRoot();
}

gkString gsGameObjectInstance::getName()
{
	return m_gobj?m_gobj->getName():"";
}

gsMesh::gsMesh(gkMesh* mesh) : m_mesh(mesh)
{}

void gsMesh::addSubMesh(gsSubMesh* submesh) {
	m_mesh->addSubMesh(submesh->m_submesh);
	// if we add the submesh to the mesh, the mesh is responsible for disposing
	// this from now on:
	submesh->m_isMeshCreator=false;
}

gsSubMesh::gsSubMesh() : m_isMeshCreator(true)
{
	m_submesh = new gkSubMesh;
}

gsSubMesh::gsSubMesh(gkSubMesh* submesh) : m_submesh(submesh), m_isMeshCreator(false)
{}

gsSubMesh::~gsSubMesh()
{
	if (m_isMeshCreator)
		delete m_submesh;
}

const gkTriangle gsSubMesh::addTriangle(const gkVertex& v0,
            unsigned int i0,
            const gkVertex& v1,
            unsigned int i1,
            const gkVertex& v2,
            unsigned int i2) {
	return m_submesh->addTriangle(v0,i0,v1,i1,v2,i2,gkTriangle::TRI_COLLIDER);
}

void gsSubMesh::setMaterialName(const gkString& materialName)
{
	m_submesh->setMaterialName(materialName);
}



bool isMaterialInitialized(const gkString& matName)
{
	Ogre::ResourcePtr resPtr = Ogre::MaterialManager::getSingleton().getByName(matName);
	if (resPtr.isNull())
		return false;
	else
		return true;
}
void initMaterial(const gkString& matName)
{
	if (!isMaterialInitialized(matName)){
		// for now we have to find the material-props by searching all submeshes!!
		// TODO: create gamekit-material-manager

		gkMesh* mesh = 0;

		gkMeshManager::ResourceIterator iter(gkMeshManager::getSingleton().getResourceIterator());
		while (iter.hasMoreElements()){
			gkMesh* mesh = static_cast<gkMesh*>(iter.getNext().second);

			gkMesh::SubMeshIterator subIter(mesh->getSubMeshIterator());
			while (subIter.hasMoreElements()) {
				gkSubMesh* subM = static_cast<gkSubMesh*>(subIter.getNext());

				if (subM->getMaterialName() == matName) {
					// init the material
					// TODO: check if there might be problems due to the group
					gkMaterialLoader::loadSubMeshMaterial(subM, mesh->getGroupName());
				}
			}

		}
	}
}

bool fileExists(const gkString& filename) {
	gkPath path(filename);
	return path.isFile();
}

gkString fileLoad(const gkString& filename) {
	gkPath path(filename);
	if (path.isFile()) {
		return path.getAsString();
	}
	return "";
}

bool import(const gkString& scriptName,bool multiCall)
{
	gkLuaScript* script = gkLuaManager::getSingleton().getByName<gkLuaScript>(scriptName);

	bool newScript = false;

	if (!script)
	{
		// first try to locate the path in filesystem
		gkString resultPath("");
		gkPath path(scriptName);

		if (path.exists())
		{
			script = gkLuaManager::getSingleton().createFromFile(path.getPath(),scriptName);
			newScript = true;
		}
		else
		{
			// try to find it relative to currentScript-path
			gkString relativeTry(gkLuaManager::locateFileRelativeToCurrentScript(scriptName));

			gkPath relativePath(relativeTry);
			if (path.exists())
			{
				script = gkLuaManager::getSingleton().createFromFile(relativePath.getPath(),scriptName);
				newScript = true;
			}
		}
	}


	if (script)
	{
		if (multiCall || newScript)
		{
			script->execute();
		}
		return true;
	}
	else
	{
		gkLogger::write("Couldn't locate import script "+scriptName+"! Neither block nor filesystem!",true);
		gkDebugScreen::printTo("Couldn't locate import script "+scriptName+"! Neither block nor filesystem!");
	}

	return false;
}

gkString getPlatform() {
#if GK_PLATFORM == GK_PLATFORM_WIN32
	   return "WIN32";
#elif GK_PLATFORM == GK_PLATFORM_APPLE
	   return  "APPLE";
#elif GK_PLATFORM == GK_PLATFORM_APPLE_IOS
	   return  "APPLE_IOS";
#elif GK_PLATFORM == GK_PLATFORM_LINUX
	   return  "LINUX";
#elif GK_PLATFORM == GK_PLATFORM_ANDROID
	return "ANDROID";
#elif GK_PLATFORM == GK_PLATFORM_BLACKBERRY
	return "BLACKBERRY";
#elif GK_PLATFORM == GK_PLATFORM_EMSCRIPTEN
	return "EMSCRIPTEN";
#else
	return "UNKNOWN";
#endif
}

bool isMobile() {
#if GK_PLATFORM==GK_PLATFORM_APPLE_IOS ||GK_PLATFORM==GK_PLATFORM_ANDROID ||GK_PLATFORM==GK_PLATFORM_BLACKBERRY
	return true;
#else
	return false;
#endif
}

bool isSoundAvailable() {
#ifdef OGREKIT_OPENAL_SOUND
	return true;
#else
	return false;
#endif
}

void infoClear(void)
{
	gkDebugScreen* console = gkDebugScreen::getConsole();
	if (console)
		console->clear();
}

void infoLock(bool showIt)
{
	gkDebugScreen* console = gkDebugScreen::getConsole();
	if (console)
		console->lock(showIt);
}

void infoShow(bool showIt)
{
	gkDebugScreen* console = gkDebugScreen::getConsole();
	if (console)
		console->show(showIt);
}

void copySceneObjects(gsScene* from, gsScene* to, int exceptObjects)
{
	gkSceneManager::getSingleton().copyObjects(from->cast<gkScene>(), to->cast<gkScene>(), exceptObjects );
}

// copy from gsEngine. why should this be in the gsEngine?
// rethink the whole lua-loading-process.
void loadBlend(const gkString& name, const gkString& sceneName,const gkString& groupName,bool ignoreLibraries)
{
	gkEngine* m_engine = gkEngine::getSingletonPtr();
	if (m_engine) // && m_ctxOwner) // && !m_running
	{
		if (!m_engine->isInitialized())
		{
			gkLogMessage("gsEngine: loadBlendFile on uninitialized engine.");
			return;
		}

		int options = sceneName.empty()?gkBlendLoader::LO_ALL_SCENES | gkBlendLoader::LO_CREATE_UNIQUE_GROUP
								:gkBlendLoader::LO_ONLY_ACTIVE_SCENE | gkBlendLoader::LO_CREATE_UNIQUE_GROUP;

		gkEngine::getSingleton().getUserDefs().ignoreLibraries = ignoreLibraries;

		gkBlendFile* gkb = gkBlendLoader::getSingleton().loadFile(gkUtils::getFile(name), options,sceneName,groupName);
		if (!gkb)
		{
			gkLogMessage("gsEngine: File Loading failed!\n");
			return;
		}
	}
}

void unloadBlendFile(const gkString& name)
{
	gkBlendLoader::getSingleton().unloadFile(name);
}

void unloadAllBlendFiles()
{
	gkBlendLoader::getSingleton().unloadAll();
}

void downloadFile(const gkString& name, const gkString& to,const gkString& postData, bool overwrite, bool SSLPeerCACheck, bool SSLHostnameCheck)
{
#ifdef OGREKIT_COMPILE_LIBCURL
	gkHttpSession::downloadToFilesystem(name,to,postData,overwrite,SSLPeerCACheck,SSLHostnameCheck);
#else
	gkLogger::write("Tried to download "+name+" but libcurl is not compiled!",true);
#endif

//	gkNetClientManager* downloadManager = gkNetClientManager::getSingletonPtr();
//	if (!downloadManager) {
//		downloadManager = new gkNetClientManager();
//	}
//	downloadManager->downloadToFilesystem(name,to,overwrite);
}

gkString downloadToString(bool blocking,const gkString& url,const gkString& postData,bool SSLPeerCACheck, bool SSLHostnameCheck) {
#ifdef OGREKIT_COMPILE_LIBCURL
	return gkHttpSession::downloadToString(blocking,url,postData,SSLPeerCACheck,SSLHostnameCheck);
#else
	gkLogger::write("Tried to download "+url+" but libcurl is not compiled!",true);
	return "NO LIBCURL";
#endif
}

void nbDownload()
{
#ifdef OGREKIT_COMPILE_LIBCURL
	  gkNetClientManager* mgr = gkNetClientManager::getSingletonPtr();
	  if (!mgr){
		  gkLogger::write("Tried to poll nonblocking but there is no server");
		  return;
	  }

	  gkHttpSession* session = (gkHttpSession*)mgr->getByName("__nb_gkdownloader__");
	  session->performMultisession();
#else
	  gkDebugScreen::printTo("NO CURL LIB! nbDownload not supported!");
#endif
}

void nbPoll()
{
#ifdef OGREKIT_COMPILE_LIBCURL
	  gkNetClientManager* mgr = gkNetClientManager::getSingletonPtr();
	  if (!mgr){
		  gkLogger::write("Tried to poll nonblocking but there is no server");
		  return;
	  }

	  gkHttpSession* session = (gkHttpSession*)mgr->getByName("__nb_gkdownloader__");
	  session->pollMultisession();
#else
	  gkDebugScreen::printTo("NO CURL LIB! nbPoll not supported!");
#endif
}

void nbSetCallback(gsFunction func)
{
#ifdef OGREKIT_COMPILE_LIBCURL
	  gkNetClientManager* mgr = gkNetClientManager::getSingletonPtr();
	  if (!mgr){
		  return;
	  }

	  gkHttpSession* session = (gkHttpSession*)mgr->getByName("__nb_gkdownloader__");

	  session->setMultisessionCallback(new gsDownloadCallback(func));
#else
	  gkDebugScreen::printTo("NO CURL LIB! nbSetCallback not supported!");
#endif
}

void nbSetCallback(gsSelf self,gsFunction func)
{
#ifdef OGREKIT_COMPILE_LIBCURL
	  gkNetClientManager* mgr = gkNetClientManager::getSingletonPtr();
	  if (!mgr){
		  return;
	  }

	  gkHttpSession* session = (gkHttpSession*)mgr->getByName("__nb_gkdownloader__");

	  session->setMultisessionCallback(new gsDownloadCallback(self,func));
#else
	  gkDebugScreen::printTo("NO CURL LIB! nbSetCallback not supported!");
#endif
}


#ifdef OGREKIT_COMPILE_LIBCURL
void gsDownloadCallback::onDone(DoneType type,utStream* stream,const gkString& url)
{
	char buf[1] = {0};
	stream->write(buf,1);
	gkString result((char*)(static_cast<utMemoryStream*>(stream)->ptr()));
	delete stream;

	m_event.beginCall();
	m_event.addArgument(result);
	m_event.addArgument(url);
	m_event.addArgument((bool)type == gkHttpSessionCallback::success);
	m_event.call();
}
#endif

bool nbIsRunning()
{
#ifdef OGREKIT_COMPILE_LIBCURL
	  gkNetClientManager* mgr = gkNetClientManager::getSingletonPtr();
	  if (!mgr){
		  return false;
	  }

	  gkHttpSession* session = (gkHttpSession*)mgr->getByName("__nb_gkdownloader__");

	  return session->isMultiSessionRunning();
#else
	  gkDebugScreen::printTo("NO CURL LIB! nbDownload not supported!");
	  return false;
#endif
}

void startNonblockingDownload()
{
#ifdef OGREKIT_COMPILE_LIBCURL
	  gkNetClientManager* mgr = gkNetClientManager::getSingletonPtr();
	  if (!mgr){
		  gkLogger::write("lua: tried to start nonblocking multicall, but there is no netmanager,yet!",true);
		  return;
	  }

	  gkHttpSession* session = (gkHttpSession*)mgr->getByName("__nb_gkdownloader__");

	  session->performMultisession();
#else
	  gkDebugScreen::printTo("NO CURL LIB! nbDownload not supported!");
#endif
}

gkGameObject* gsProcedural::createBox(const gkString& name,Ogre::Real sizeX, Ogre::Real sizeY, Ogre::Real sizeZ, unsigned int numSegX, unsigned int numSegY, unsigned int numSegZ)
{
	Procedural::BoxGenerator(sizeX,sizeY,sizeZ,numSegX,numSegY,numSegZ).realizeMesh(name);
	gkEntity* result = gkEngine::getSingleton().getActiveScene()->createEntity(name);
	result->setExternalMeshName(name);
	result->createInstance(false);
	return result;
}

gkGameObject* gsProcedural::createRoundedBox(const gkString& name,Ogre::Real sizeX, Ogre::Real sizeY, Ogre::Real sizeZ, unsigned int numSegX, unsigned int numSegY, unsigned int numSegZ, float c)
{
	Procedural::RoundedBoxGenerator().setSizeX(sizeX).setSizeY(sizeY).setSizeZ(sizeZ).setNumSegX(numSegX).setNumSegY(numSegY)
			.setNumSegZ(numSegZ).setChamferSize(c).realizeMesh(name);
	gkEntity* result = gkEngine::getSingleton().getActiveScene()->createEntity(name);
	result->setExternalMeshName(name);
	result->createInstance(false);
	return result;
}

//
void ribbon(gsGameObject* gobj,float width,float widthChange,int length,int maxElements,const gkString& materialName,gsVector3 initCol,gsVector4 colChange)
{


	Ogre::SceneManager* mgr = gkEngine::getSingleton().getActiveScene()->getManager();

	Ogre::NameValuePairList pairList;
	pairList["numberOfChains"] = "1";
	pairList["maxElements"] = gkToString(maxElements);

	Ogre::RibbonTrail* trail = static_cast<Ogre::RibbonTrail*>(mgr->createMovableObject("1", "RibbonTrail", &pairList));
	trail->setMaterialName(materialName);
	trail->setTrailLength(length);
	mgr->getRootSceneNode()->createChildSceneNode()->attachObject(trail);
	trail->setInitialColour(0, initCol.x,initCol.y,initCol.z);
	trail->setColourChange(0, colChange.x,colChange.y,colChange.z,colChange.w);
	trail->setInitialWidth(0,width);
	trail->addNode(gobj->get()->getNode());
	trail->setWidthChange(0,widthChange);


	/*
	Procedural::PlaneGenerator().setNumSegX(20).setNumSegY(20).setSizeX(150).setSizeY(150).setUTile(5.0).setVTile(5.0).realizeMesh("planeMesh");
    Procedural::SphereGenerator().setRadius(2.f).setUTile(5.).setVTile(5.).realizeMesh("sphereMesh");
	Procedural::CylinderGenerator().setHeight(3.f).setRadius(1.f).setUTile(3.).realizeMesh("cylinderMesh");
	Procedural::TorusGenerator().setRadius(3.f).setSectionRadius(1.f).setUTile(10.).setVTile(5.).realizeMesh("torusMesh");
	Procedural::ConeGenerator().setRadius(2.f).setHeight(3.f).setNumSegBase(36).setNumSegHeight(2).setUTile(3.).realizeMesh("coneMesh");

	gkEntity* plane = gkEngine::getSingleton().getActiveScene()->createEntity("plane");
	plane->setExternalMeshName("cylinderMesh");
	plane->createInstance(true);

	Procedural::Path p;
		p.addPoint(0, 0, 0);
		p.addPoint(0, 10, 0);
		p.addPoint(10, 20, 0);

		Procedural::MultiShape out;
		Procedural::SvgLoader svg;
		svg.parseSvgFile(out, "test.svg", "General", 16);
		Procedural::Extruder().setMultiShapeToExtrude(&out).setExtrusionPath(&p).setScale(.05f).realizeMesh("svg");

	gkEntity* torusMesh = gkEngine::getSingleton().getActiveScene()->createEntity("torus");
	torusMesh->setExternalMeshName("svg");
	torusMesh->createInstance(true);

	torusMesh->translate(gkVector3(1.0f,1.0f,1.0f));
*/
}


void addResourceLocation(const gkString& group, const gkString& type, const gkString& path, bool recursive)
{
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path,type,group,recursive);

}

void loadResource(const gkString& group,const gkString& path)
{
	Ogre::FileInfoListPtr findRes = Ogre::ResourceGroupManager::getSingleton().findResourceFileInfo(group,path);

	for (Ogre::FileInfoList::iterator iter = findRes->begin();iter!=findRes->end();iter++)
	{
		Ogre::FileInfo fi = *iter;
		gkString fileName = fi.filename;
		gkString filePath = fi.path;
		gkString base = fi.basename;
		int b = 2;
	}
	int findResSize = findRes->size();


	Ogre::StringVectorPtr listRes = Ogre::ResourceGroupManager::getSingleton().listResourceNames(group);
	int listResSize = listRes->size();

	Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(path,group);



//	Ogre::StringVectorPtr listResDir = Ogre::ResourceGroupManager::getSingleton().listResourceNames(group,true);
//	int size = findRes->size();
//	Ogre::StringVectorPtr listRes = Ogre::ResourceGroupManager::getSingleton().listResourceNames(group);
//	int size = findRes->size();

	int a = 0;
}

void log(const gkString& log) {
	gkLogger::write(log,true);
}

gkString md5(const gkString& md5) {
	return gkCrypt::md5(md5);
}

gkString blowfishEncrypt(const gkString& key, const gkString& in){
	return gkCrypt::blowfishEncrypt(key,in);
}

gkString blowfishDecrypt(const gkString& key, const gkString& in){
	return gkCrypt::blowfishDecrypt(key,in);
}

bool isNetworkAvailable(const gkString& url)
{
#ifdef OGREKIT_COMPILE_LIBCURL
	return gkNetworkUtils::isNetworkAvailable(url);
#else
	gkDebugScreen::printTo("No libcurl! isNetworkAvailable not available");
	return false;
#endif
}

class AsyncNetworkCheck : public gkCall
{
public:
	AsyncNetworkCheck(gsSelf self, gsFunction func, const gkString& url)
		: m_url(url)
	{
		m_event = new gkPreparedLuaEvent(self,func);
	}

	void run() {
		bool result = isNetworkAvailable(m_url);
//		gsEngine::getSingleton().__blockTick();

		m_event->addArgument(result);
		gkLuaManager::getSingleton().addPreparedLuaEvent(m_event);
	}
private:
	gkPreparedLuaEvent* m_event;
	gkString m_url;
};


void asyncIsNetworkAvailable(gsSelf self,gsFunction func,const gkString& url)
{
#ifdef OGREKIT_COMPILE_LIBCURL
	gkNetClientManager::getSingleton().getNetworkHelper().enqueue(gkPtrRef<gkCall>(new AsyncNetworkCheck(self,func,url)));
#else
	gkDebugScreen::printTo("WARNING! no libcurl! asyncIsNetworkAvailable not supported!!");
#endif
}

int scancodeToInt(gkScanCode code)
{
	return (int) code;
}

gkString getScriptFolder() {
	if (gkLuaManager::CURRENTSCRIPT->isCreatedFromFile()) {
		return gkLuaManager::CURRENTSCRIPT->getFilePath();
	} else {
		gkDebugScreen::printTo("Warning: current script is not created from file");
		return "SCRIPT NO FILE!";
	}
}


unsigned long getTime() {
	return gkEngine::getSingleton().getTickClock()->getTimeMilliseconds();
}

#ifdef WIN32

#include <sys/time.h>


#endif


unsigned long getSystemTime() {

	timeval time;
#ifdef EMSCRIPTEN
	gkLogger::write("lua: getSystemTime() for emscripten is not available", true);
	return 0;
#else
	gettimeofday(&time, NULL);
	long millis = (time.tv_sec * 1000) + (time.tv_usec / 1000);
	return millis;
#endif
}

gsVector2 getScreenSize()
{

	return gsVector2(gkWindowSystem::getSingleton().getMainRenderWindow()->getWidth(),
					  gkWindowSystem::getSingleton().getMainRenderWindow()->getHeight());
}

gkString getDataDirectory(const gkString& folder)
{
#if GK_PLATFORM == GK_PLATFORM_APPLE_IOS
	   return  "APPLE_IOS";
#elif GK_PLATFORM == GK_PLATFORM_ANDROID
		gkPath path("");
		return path.getSDCardPath()+"/"+folder;
#elif GK_PLATFORM == GK_PLATFORM_BLACKBERRY
	return "./data";
#else
	gkPath current(folder);
	return current.getAbsPath();
#endif
}

float getFPS()
{
	Ogre::RenderWindow* window = gkWindowSystem::getSingleton().getMainWindow()->getRenderWindow();
	const Ogre::RenderTarget::FrameStats& ogrestats = window->getStatistics();
	return ogrestats.lastFPS;
}

gsStaticBatch::~gsStaticBatch()
{
	if (mBatch)
	{
		Ogre::SceneManager* mgr = mScene->getManager();
		if (mgr)
			mgr->destroyStaticGeometry(mBatch);

		mBatch = 0;
	}
}

void gsStaticBatch::_addEntityWithChildren(gkGameObject* obj)
{
	if (obj->getType() == GK_ENTITY && !obj->hasVariable("gk_no_batch"))
	{
		gkEntity* ent = obj->getEntity();
		Ogre::Entity* ogent = ent->getEntity();

		if (!ogent)
			return;

		const gkGameObjectProperties& props = obj->getProperties();

		if (!obj->getPhysicsController()
				|| obj->getPhysicsController()->isStaticObject())
		{
			mBatch->addEntity(ogent,
			                      obj->getWorldPosition(),
			                      obj->getWorldOrientation(),
			                      obj->getWorldScale());
			// no longer needed
//			obj->getNode()->getParent()->removeChild(obj->getNode());
			ent->_destroyAsStaticGeometry();
		}


		gkGameObjectArray::Iterator children(obj->getChildren());
		while (children.hasMoreElements())
		{
			gkGameObject* gobj = children.getNext();
			_addEntityWithChildren(gobj);
		}

	}

}

void gsStaticBatch::addEntity(gsEntity* ent)
{
	if (mStaticEntityList.find(ent)==0)
		mStaticEntityList.push_back(ent);
}

void gsStaticBatch::removeEntity(gsEntity* ent)
{
	mStaticEntityList.erase(ent);
}

void gsStaticBatch::build()
{
	Ogre::SceneManager* mgr = mScene->getManager();
	if (mBatch) {
		mgr->destroyStaticGeometry(mBatch);
	}
	mBatch = mgr->createStaticGeometry(gkUtils::getUniqueName("staticbatch"));

	StaticEntityList::Iterator iter(mStaticEntityList);
	while (iter.hasMoreElements())
	{
		_addEntityWithChildren(iter.getNext()->get());
	}

	mBatch->build();
}

gkString getWindowTitle()
{
	return gkEngine::getSingleton().getUserDefs().wintitle;
};

void logDisable()
{
	gkLogger::disable();
}
void logEnable(bool verbose)
{
	gkLogger::enable("gamekit",verbose);
}




gkString compressString(const gkString& inputString, gsCompressionLevel compLevel)
{
	z_stream zs;                        // z_stream is zlib's control structure
    memset(&zs, 0, sizeof(zs));
    int cL = (int)compLevel;
    if (deflateInit(&zs, compLevel) != Z_OK)
        throw(std::runtime_error("deflateInit failed while compressing."));
    zs.next_in = (Bytef*)inputString.data();
    zs.avail_in = inputString.length();           // set the z_stream's input

    int ret;
    char outbuffer[32768];
    std::string outstring;

    // retrieve the compressed bytes blockwise
    do {
        zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
        zs.avail_out = sizeof(outbuffer);

        ret = deflate(&zs, Z_FINISH);

        if (outstring.size() < zs.total_out) {
            // append the block to the output string
            outstring.append(outbuffer,
                             zs.total_out - outstring.size());
        }
    } while (ret == Z_OK);

    deflateEnd(&zs);

    if (ret != Z_STREAM_END) {          // an error occurred that was not EOF
        std::ostringstream oss;
        gkLogger::write(oss.str(),true);
        return "";
    }

    return outstring;
}

const char* compressCString(char* input,int& len, gsCompressionLevel compLevel)
{
	z_stream zs;                        // z_stream is zlib's control structure
    memset(&zs, 0, sizeof(zs));
    int cL = (int)compLevel;
    if (deflateInit(&zs, compLevel) != Z_OK)
        throw(std::runtime_error("deflateInit failed while compressing."));
    zs.next_in = (Bytef*)input;
    zs.avail_in = len;           // set the z_stream's input

    int ret;
    char outbuffer[32768];
    std::string outstring;

    utMemoryStream result;
    result.reserve(len);
    // retrieve the compressed bytes blockwise
    do {
        zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
        zs.avail_out = sizeof(outbuffer);

        ret = deflate(&zs, Z_FINISH);

        if (outstring.size() < zs.total_out) {
            // append the block to the output string
            result.write(outbuffer,zs.total_out - outstring.size());
//        	outstring.append(outbuffer,
//                             zs.total_out - outstring.size());
        }
    } while (ret == Z_OK);

    deflateEnd(&zs);

    if (ret != Z_STREAM_END) {          // an error occurred that was not EOF
        std::ostringstream oss;
        gkLogger::write(oss.str(),true);
        return "";
    }

    char* finalResult = new char[result.position()];
    memcpy(finalResult,result.ptr(),result.position());
    len = result.position();
    return finalResult;
}

gkString uncompressString(const gkString& data)
{
    z_stream zs;                        // z_stream is zlib's control structure
    memset(&zs, 0, sizeof(zs));

    if (inflateInit(&zs) != Z_OK)
    {
    	gkLogger::write("inflateInit failed wit decompressing!",true);
    	return "";
    }

    zs.next_in = (Bytef*)data.data();
    zs.avail_in = data.length();

    int ret;
    char outbuffer[32768];
    std::string outstring;

    // get the decompressed bytes blockwise using repeated calls to inflate
    do {
        zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
        zs.avail_out = sizeof(outbuffer);

        ret = inflate(&zs, 0);

        if (outstring.size() < zs.total_out) {
            outstring.append(outbuffer,
                             zs.total_out - outstring.size());
        }

    } while (ret == Z_OK);

    inflateEnd(&zs);

    if (ret != Z_STREAM_END) {          // an error occurred that was not EOF

    	std::ostringstream oss;
        oss << "Exception during zlib decompression: (" << ret << ") "
            << zs.msg;

        gkLogger::write(oss.str());
        return outstring;
    }

    return outstring;

}


gkString gsLuaStream::read(int amount)
{
	char* buf = new char[amount];
	int read=0;

	while (read<amount)
		read += m_stream.read(buf,amount);

	gkString result(buf,0,read);
	delete[] buf;
	return result;
}

void setScreenSize(int width,int height)
{
	Ogre::RenderWindow* win = gkWindowSystem::getSingleton().getMainWindow()->getRenderWindow();
	win->resize(width,height);
}


void executeScript(const gkString& luaScript)
{
	gkLuaScript* luascript = gkLuaManager::getSingleton().createFromText(gkUtils::getUniqueName("inline-script"),luaScript);
	luascript->execute();
	gkLuaManager::getSingleton().destroy(luascript);
}

gkString getWorkingFolder()
{
	gkString path;
	gkGetCurrentDir(path);
	return path;
}


void sendEventMessage(const gkString& type, const gkString& value,const gkString& ext,bool queued)
{
	// IEventDataPtr will take care of this pointer
	gkMessageEvent* msgEvent = new gkMessageEvent(type);

	gkVariable varValue(value,"value");
	gkVariable varExt(ext,"ext");
	msgEvent->setData("value",&varValue);
	msgEvent->setData("ext",&varExt);
	safeQueueEvent(IEventDataPtr(msgEvent));
}

void setSinglestepMode(bool singlestepMode)
{
	gkEngine::getSingleton().setSingleStepMode(singlestepMode);
}
bool inSinglestepMode()
{
	return gkEngine::getSingleton().inSinglestepMode();
}
void requestSinglestep()
{
	gkEngine::getSingleton().requestSinglestep();
}


// ---------------------------- NATIVE LUA --------------------------------------------------------
// native lua functions, no wrapping:
int native_getScreenshotAsString(lua_State*L) // my native code
{
	Ogre::RenderWindow* win = gkWindowSystem::getSingleton().getMainWindow()->getRenderWindow();

	Ogre::PixelFormat pf = win->suggestPixelFormat();

	int width = win->getWidth();
	int height = win->getHeight();

	int size = width * height * 4;
	char* data = new char[size];

	gkString str;

	Ogre::PixelBox pb(win->getWidth(), win->getHeight(), 1, Ogre::PF_BYTE_RGBA, data);
	win->copyContentsToMemory(pb);

	const char* compressionResult = 0;
	int compSize = size;
	// compress
	if (true)
	{
		compressionResult = compressCString(data,compSize,COMPRESSION_BEST);

//		compressCString(char* input,int& len, gsCompressionLevel compLevel)

//		int compLevel = 1;
//		z_stream zs;                        // z_stream is zlib's control structure
//	    memset(&zs, 0, sizeof(zs));
//	    int cL = (int)compLevel;
//	    if (deflateInit(&zs, compLevel) != Z_OK)
//	        throw(std::runtime_error("deflateInit failed while compressing."));
//	    zs.next_in = (Bytef*)data;
//	    zs.avail_in = size;           // set the z_stream's input
//
//	    int ret;
//	    char outbuffer[32768];
//	    std::string outstring;
//
//	    utMemoryStream result;
//	    result.reserve(size);
//	    // retrieve the compressed bytes blockwise
//	    do {
//	        zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
//	        zs.avail_out = sizeof(outbuffer);
//
//	        ret = deflate(&zs, Z_FINISH);
//
//	        if (outstring.size() < zs.total_out) {
//	            // append the block to the output string
//	            result.write(outbuffer,zs.total_out - outstring.size());
//	//        	outstring.append(outbuffer,
//	//                             zs.total_out - outstring.size());
//	        }
//	    } while (ret == Z_OK);
//
//	    deflateEnd(&zs);
//
//	    if (ret != Z_STREAM_END) {          // an error occurred that was not EOF
//	        std::ostringstream oss;
//	        gkLogger::write(oss.str(),true);
//	        return "";
//	    }
//
//	    char* finalResult = new char[result.position()];
//	    memcpy(finalResult,result.ptr(),result.position());
//	    len = result.position();
	}
//	const char* compressed = compressCString(
//			data,
//			size,
//			COMPRESSION_FAST);


	if (compressionResult)
	{
		gkLogger::write("uncomp:"+gkToString((int)size)+" comp:"+gkToString((int)compSize));
		lua_pushlstring(L,compressionResult,compSize);
		delete[] compressionResult;
	}
	else
	{
		lua_pushlstring(L,data,size);
	}

	gkEngine::getSingleton().saveTimestampedScreenShot("tom","png");

	gkLogger::write("Result Length:"+gkToString(size),true);

    delete[] data;

	return 1;
}


// ------------------------ NATIVE LUA FUNCTIONS END ------------------------------------
