/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2010 Charlie C.

    Contributor(s): xat.
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

#include <External/Ogre/gkOgreVideoBackground.h>
#include <gkEventManager.h>
#include "gkEngine.h"
#include "gkWindowSystem.h"
#include "gkWindow.h"
#include "gkScene.h"
#include "gkSceneManager.h"
#include "gkLogger.h"
#include "gkUtils.h"
#include "gkLogicManager.h"
#include "gkBlendFile.h"
#include "gkBlendLoader.h"
#include "gkRenderFactory.h"
#include "gkUserDefs.h"
#include "gkTextManager.h"
#include "gkDynamicsWorld.h"
#include "gkDebugScreen.h"
#include "gkDebugProperty.h"
#include "gkTickState.h"
#include "gkDebugFps.h"
#include "gkStats.h"
#include "gkMessageManager.h"
#include "gkMeshManager.h"
#include "gkSkeletonManager.h"
#include "gkGroupManager.h"
#include "gkGameObjectManager.h"
#include "gkResourceGroupManager.h"
#include "gkAnimationManager.h"
#include "gkParticleManager.h"
#include "gkHUDManager.h"

#include "OgrePixelFormat.h"

#include "OgreRenderWindow.h"

#include "OgreRenderTarget.h"
#ifdef BLACKBERRY
# include <screen/screen.h>
#endif

#ifdef OGREKIT_USE_NNODE
#include "gkNodeManager.h"
#endif

#ifdef OGREKIT_USE_LUA
#include "Script/Lua/gkLuaManager.h"
#endif

#ifdef OGREKIT_OPENAL_SOUND
# include "Sound/gkSoundManager.h"
#endif

#if defined(OGREKIT_COMPILE_OGRE_SCRIPTS) || defined(OGREKIT_COMPILE_LIBROCKET)
# include "gkFontManager.h"
#endif

#include "External/Ogre/gkOgreBlendArchive.h"

#ifdef OGREKIT_COMPILE_LIBROCKET
#include <gkGUIManager.h>
#endif

#ifdef OGREKIT_NETWORK_ENET_SUPPORT
# include "Network/gkNetworkManager.h"
#endif


#ifdef OGREKIT_USE_COMPOSITOR
#include "External/Ogre/gkOgreCompositorManager.h"
#endif

#ifdef OGREKIT_COMPILE_LIBCURL
# include "Network/gkDownloadManager.h"
#endif

#include "OgreRoot.h"
#include "OgreConfigFile.h"
#include "OgreRenderSystem.h"
#include "OgreStringConverter.h"
#include "OgreFrameListener.h"
#include "OgreOverlayManager.h"

// temporary hack for keeping compatibility with ogre18 due to the android-version
#ifndef BUILD_OGRE18
#include "OgreOverlaySystem.h"
#endif

#ifdef OGREKIT_USE_RTSHADER_SYSTEM
# include "OgreRTShaderSystem.h"
#endif

#ifdef OGREKIT_USE_THEORA
	#include "OgreExternalTextureSourceManager.h"
	#include "OgreVideoManager.h"
	#include "External/Ogre/gkOgreVideoBackground.h"

	using Ogre::OgreVideoManager;
	using Ogre::ExternalTextureSourceManager;
#endif



// shorthand
#define gkOgreEnginePrivate			gkEngine::Private
#define ENGINE_TICKS_PER_SECOND		gkScalar(60)

// tick states
gkScalar gkEngine::m_tickRate = ENGINE_TICKS_PER_SECOND;


class gkOgreEnginePrivate : public Ogre::FrameListener, public gkTickState
{
public:
	Private(gkEngine* par)
		:       gkTickState(par->getTickRate()),
		        engine(par),
		        windowsystem(0),
		        curScene(0),
		        debug(0),
		        debugPage(0),
		        debugFps(0),
				archive_factory(0),
				timer(0),
				root(0),
				overlaySystem(0)

	{
		timer = new btClock();
		timer->reset();
		curTime = timer->getTimeMilliseconds();

		plugin_factory = new gkRenderFactoryPrivate();
		archive_factory = new gkBlendArchiveFactory();
	}

	virtual ~Private()
	{
		delete timer;
		delete plugin_factory;
		delete archive_factory;
	}


	// one full update
	void tickImpl(gkScalar delta);
	void beginTickImpl(void);
	void endTickImpl(void);


	bool frameStarted(const Ogre::FrameEvent& evt);
	bool frameRenderingQueued(const Ogre::FrameEvent& evt);
	bool frameEnded(const Ogre::FrameEvent& evt);

	gkEngine*                   engine;
	gkWindowSystem*             windowsystem;       // current window system
	gkScene*                    curScene;			// current scene
	gkSceneArray				scenes;
	gkRenderFactoryPrivate*     plugin_factory;     // static plugin loading
	Ogre::Root*                 root;
	gkDebugScreen*              debug;
	gkDebugPropertyPage*        debugPage;
	gkDebugFps*                 debugFps;
	
	btClock*					timer;
	unsigned long				curTime;

	gkBlendArchiveFactory*		archive_factory;

#ifndef BUILD_OGRE18
	Ogre::OverlaySystem*		overlaySystem;
#endif
};






gkEngine::gkEngine(gkUserDefs* oth)
	:	m_window(0),
		m_initialized(false),
		m_ownsDefs(oth != 0),
		m_running(false),
		m_scriptListenerSet(false),
		gkMessageManager::GenericMessageListener("",GK_MSG_GROUP_INTERNAL,""),
		m_singlestepMode(false),
		m_doStep(true),
		m_requestStep(false)
{
	m_private = new gkOgreEnginePrivate(this);
	if (oth != 0)
		m_defs = oth;
	else
		m_defs = new gkUserDefs();
}




gkEngine::~gkEngine()
{

	if (m_initialized)
		finalize();


	// persistent throughout
	gkLogger::disable();

	if (!m_ownsDefs)
	{
		delete m_defs;
		m_defs = 0;
	}
}

void ogrevideo_log(std::string message)
{
	Ogre::LogManager::getSingleton().logMessage("OgreVideo: "+message);
}


void gkEngine::initialize()
{
	if (m_initialized) return;

	new gkEventManager("EventManager",true);
//	safeAddListener(IEventListenerPtr(new TestListener()),"*");

	gkUserDefs& defs = getUserDefs();
	gkLogger::enable(defs.log, defs.verbose);

	if (defs.rendersystem == OGRE_RS_UNKNOWN)
	{
		gkPrintf("Unknown rendersystem!\n");
		return;
	}

	Ogre::Root* root = new Ogre::Root("", "");
	m_private->root = root;
	m_private->plugin_factory->createRenderSystem(root, defs.rendersystem);
	m_private->plugin_factory->createParticleSystem(root);
	m_private->plugin_factory->createCGProgrammManager(root);
	m_private->archive_factory->addArchiveFactory();

#ifndef BUILD_OGRE18
	m_private->overlaySystem = new Ogre::OverlaySystem();
#endif
	const Ogre::RenderSystemList& renderers = root->getAvailableRenderers();
	if (renderers.empty())
	{
		gkPrintf("No rendersystems present\n");
		return;
	}
	//gkLogger::write("Using renderer:"+renderers[0]->getName());
	root->setRenderSystem(renderers[0]);
#if defined(_MSC_VER) && defined(OGRE_BUILD_RENDERSYSTEM_GLES2)
	renderers[0]->setConfigOption("RTT Preferred Mode", "Copy"); //angleproject gles2
#endif
#ifdef BLACKBERRY
	if (m_defs->_bbScreen)
		root->setBlackberryScreen(*m_defs->_bbScreen);
#endif
	root->initialise(false);

	m_private->windowsystem = new gkWindowSystem();

#ifdef OGREKIT_COMPILE_LIBCURL
	if (!gkNetClientManager::getSingletonPtr())
		new gkNetClientManager();
#endif

	// gk Managers
	new gkResourceGroupManager();
	new gkSceneManager();
#ifdef OGREKIT_USE_NNODE
	new gkNodeManager();
#endif
	new gkBlendLoader();
	new gkTextManager();
	new gkMessageManager();
	gkMessageManager::getSingleton().addListener(this);

	new gkMeshManager();
	new gkSkeletonManager();
#ifdef OGREKIT_USE_PARTICLE
	new gkParticleManager();
#endif
	new gkHUDManager();
	new gkGroupManager();
	new gkGameObjectManager();

	new gkAnimationManager();



#ifdef OGREKIT_USE_LUA
    if (!gkLuaManager::getSingletonPtr())
    	new gkLuaManager();
#endif
#if defined(OGREKIT_COMPILE_OGRE_SCRIPTS) || defined(OGREKIT_COMPILE_LIBROCKET)
	new gkFontManager();
#endif
#ifdef OGREKIT_COMPILE_LIBROCKET
	new gkGUIManager();
#endif
#ifdef OGREKIT_OPENAL_SOUND
	new gkSoundManager();
#endif

#ifdef OGREKIT_NETWORK_ENET_SUPPORT
        new gkNetworkManager;
//      gkMessageNetForwarder* forwarder = new gkMessageNetForwarder;
//      gkNetworkManager::getSingleton().addClientListener(static_cast<gkNetworkListener*>(forwarder));
//      gkNetworkManager::getSingleton().startClient("192.168.0.159",1035);
//      gkMessageManager::getSingleton().addListener(static_cast<gkMessageManager::MessageListener*>(forwarder));
#endif

#ifdef OGREKIT_USE_COMPOSITOR
	new gkCompositorManager();
#endif
	//gkLogger::write("37",true);

	initializeWindow();
	if (!defs.resources.empty())
	{
		loadResources(defs.resources);
	}
#ifdef OGREKIT_USE_RTSHADER_SYSTEM	
	defs.hasFixedCapability = renderers[0]->getCapabilities()->hasCapability(Ogre::RSC_FIXED_FUNCTION);


	gkResourceGroupManager::getSingleton().initRTShaderSystem(
		m_private->plugin_factory->getShaderLanguage(), defs.shaderCachePath, defs.hasFixedCapability);
#endif

	// create the builtin resource group
	gkResourceGroupManager::getSingleton().createResourceGroup(GK_BUILTIN_GROUP);

	gkResourceGroupManager::getSingleton().initialiseAllResourceGroups();

#ifdef OGREKIT_USE_PARTICLE
	gkParticleManager::getSingleton().initialize();
#endif

#if defined OGREKIT_USE_THEORA
	OgreVideoManager* theoraVideoPlugin = new OgreVideoManager();
	TheoraVideoManager::setLogFunction(ogrevideo_log);
	// Register with Manager
	ExternalTextureSourceManager::getSingleton().setExternalTextureSource("ogg_video",theoraVideoPlugin);
	root->addFrameListener(theoraVideoPlugin);

	gkVideoBackground* vm = new gkVideoBackground();
#endif

#ifdef OGREKIT_USE_COMPOSITOR
	gkCompositorManager::getSingleton().initialize();
#endif

	// debug info
	m_private->debug = new gkDebugScreen();
	m_private->debug->initialize();
	m_private->debug->show(true);

	m_private->debugPage = new gkDebugPropertyPage();
	m_private->debugPage->initialize();

	m_private->debugFps = new gkDebugFps();
	m_private->debugFps->initialize();
	m_private->debugFps->show(defs.debugFps);


	// statistics and profiling
	new gkStats();


	import("lua/lua-libs/boot.lua");

	// do we have injected lua that should be called before the game starts? (e.g. by passing via -e argument on the runtime)
	if (!defs.injectedLua.empty())
	{
		gkLuaScript* injectedLuaScript = gkLuaManager::getSingleton().createFromText("injectedLua", defs.injectedLua+"  ");
		if (defs.verbose)
		{
			gkLogger::write("-----------Injected LUA START------------");
			gkLogger::write(defs.injectedLua);
			gkLogger::write("-----------Injected LUA END  ------------");
		}
		injectedLuaScript->execute();
	}

	// prepare script to be called every time a scene gets instantiated. Just call the global function __onSceneCreated() which is defined in Engine/User/codescripts/lua/lua-libs/boot.lua
	gkLuaManager::getSingleton().createFromText("__onSceneCreated","__onSceneCreated()  ");

	m_initialized = true;
}




void gkEngine::initializeWindow(void)
{
	if (m_private->windowsystem && !m_window)
	{
		gkWindowSystem* sys = m_private->windowsystem;
		gkUserDefs& defs = getUserDefs();

		m_window = sys->createWindow(defs);
	}
}





void gkEngine::finalize()
{
	if (!m_initialized) return;	

#ifdef OGREKIT_OPENAL_SOUND
	gkSoundManager::getSingleton().stopAllSounds();
#endif	

	gkResourceManager* tmgr;

#ifdef OGREKIT_USE_NNODE
	tmgr = gkNodeManager::getSingletonPtr();
	tmgr->destroyAll();
#endif

	tmgr = gkSceneManager::getSingletonPtr();
	tmgr->destroyAll();


	tmgr = gkGroupManager::getSingletonPtr();
	tmgr->destroyAll();

	tmgr = gkGameObjectManager::getSingletonPtr();
	tmgr->destroyAll();


#ifdef OGREKIT_NETWORK_ENET_SUPPORT
    delete gkNetworkManager::getSingletonPtr();
#endif

#ifdef OGREKIT_USE_NNODE
	delete gkNodeManager::getSingletonPtr();
#endif
	delete gkGroupManager::getSingletonPtr();
	delete gkGameObjectManager::getSingletonPtr();
	delete gkSceneManager::getSingletonPtr();

	gkLogicManager::deleteManagers();

	delete gkTextManager::getSingletonPtr();
	delete gkWindowSystem::getSingletonPtr();
	delete gkMessageManager::getSingletonPtr();
	delete gkMeshManager::getSingletonPtr();
	delete gkSkeletonManager::getSingletonPtr();
#ifdef OGREKIT_USE_PARTICLE
	delete gkParticleManager::getSingletonPtr();
#endif

#if defined OGREKIT_USE_THEORA
	// TODO: any need to unregister the framelistener? since we kick the whole engine I assume not...
	delete OgreVideoManager::getSingletonPtr();
	delete gkVideoBackground::getSingletonPtr();
#endif

	delete gkHUDManager::getSingletonPtr();
	delete gkAnimationManager::getSingletonPtr();

#ifdef OGREKIT_COMPILE_LIBCURL
	if (gkNetClientManager::getSingletonPtr())
		delete gkNetClientManager::getSingletonPtr();
#endif

#ifdef OGREKIT_USE_LUA
	delete gkLuaManager::getSingletonPtr();
#endif

#if defined(OGREKIT_COMPILE_OGRE_SCRIPTS) || defined(OGREKIT_COMPILE_LIBROCKET)
	delete gkFontManager::getSingletonPtr();
#endif
	
#ifdef OGREKIT_OPENAL_SOUND
	delete gkSoundManager::getSingletonPtr();
#endif

#ifdef OGREKIT_USE_RTSHADER_SYSTEM
	Ogre::MaterialManager::getSingleton().setActiveScheme(Ogre::MaterialManager::DEFAULT_SCHEME_NAME);
	Ogre::RTShader::ShaderGenerator::destroy();
#endif

#ifdef OGREKIT_USE_COMPOSITOR
	delete gkCompositorManager::getSingletonPtr();
#endif

	delete gkBlendLoader::getSingletonPtr();
	delete gkResourceGroupManager::getSingletonPtr();


	delete gkStats::getSingletonPtr();
	delete m_private->debugFps;
	delete m_private->debugPage;
	delete m_private->debug;
#ifndef BUILD_OGRE18
	delete m_private->overlaySystem;
#endif
	delete m_private->root;
	delete m_private;

	m_initialized = false;

	delete gkEventManager::Get();
}




gkUserDefs& gkEngine::getUserDefs(void)
{
	GK_ASSERT(m_defs);
	return *m_defs;
}



void gkEngine::requestExit(void)
{
	// TODO: For some reason this results in an error. but it should be still in a stable state!??
//	gkMessageManager::getSingleton().sendMessage(GK_MSG_GROUP_INTERNAL,"","request_exit","");
	gkWindowSystem::getSingleton().exit(true);
}

gkString gkEngine::screenshootAsString(int width,int height)
{
	gkWindowSystem* sys = m_private->windowsystem;
	if (!sys)
	{
		gkLogMessage("Engine: Can't take screenshot without a window system. error\n");
		return "";
	}


	gkWindow* rwin = sys->getMainWindow();

	rwin->getRenderWindow()->resize(width,height);

	Ogre::RenderWindow* win = rwin->getRenderWindow();

	Ogre::PixelFormat pf = win->suggestPixelFormat();

	width = win->getWidth();
	height = win->getHeight();


	int size = width * height * 4;
	char* data = new char[size];

	gkString str;

	Ogre::PixelBox pb(win->getWidth(), win->getHeight(), 1, Ogre::PF_BYTE_RGBA, data);
	win->copyContentsToMemory(pb);

	gkString result(data,0,size);
	int sizeTest = result.size();
	int length = result.length();
	//gkLogger::write("Result Length:"+gkToString((int)result.length())+" size:"+gkToString(size));

    delete[] data;

	return result;
}




void gkEngine::saveTimestampedScreenShot(const gkString& filenamePrefix, const gkString& filenameSuffix)
{
	gkWindowSystem* sys = m_private->windowsystem;
	if (!sys)
	{
		gkLogMessage("Engine: Can't take screenshot without a window system. error\n");
		return;
	}

	gkWindow* rwin = sys->getMainWindow();
	if (!rwin)
	{
		gkLogMessage("Engine: Can't take screenshot without a render window. error\n");
		return;
	}
	rwin->writeContentsToTimestampedFile(filenamePrefix, filenameSuffix);
}



gkBlendFile* gkEngine::loadBlendFile(const gkString& blend, int options, const gkString& group)
{
	// This function is not really needed any more.
	// just use gkBlendLoader::getSingleton()
	return gkBlendLoader::getSingleton().loadFile(blend, options, "", group);
}



void gkEngine::loadResources(const gkString& name)
{
#if GK_PLATFORM == GK_PLATFORM_ANDROID
	gkPath::initAssetManager();
#endif

	if (name.empty()) return;

	try
	{
		Ogre::ConfigFile fp;
#if GK_PLATFORM == GK_PLATFORM_ANDROID
		gkPath path("res.cfg");
		if (path.androidExsitsInAssets())
		{
			// super hacky
			// TODO: One filesystem refactor this have to be changed
			try {
				Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource("res.cfg");
				fp.load(stream);
			}catch (...)
			{
				//gkLogger::write("Loading via ogre didn'T work out");
			}

		}
#else
		fp.load(name);
#endif

		Ogre::ResourceGroupManager* resourceManager = Ogre::ResourceGroupManager::getSingletonPtr();
		Ogre::ConfigFile::SectionIterator cit = fp.getSectionIterator();

		while (cit.hasMoreElements())
		{
			gkString elementname = cit.peekNextKey();
			Ogre::ConfigFile::SettingsMultiMap* ptr = cit.getNext();
			for (Ogre::ConfigFile::SettingsMultiMap::iterator dit = ptr->begin(); dit != ptr->end(); ++dit)
			{
				bool recursive = (dit->first == "Zip" || dit->first == "APKZip");

				resourceManager->addResourceLocation(dit->second, dit->first, elementname, recursive);
			}

		}
		Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	}
	catch (Ogre::Exception& e)
	{
		gkLogMessage("Engine: Failed to load resource file!\n" << e.getDescription());
	}
}



void gkEngine::addDebugProperty(gkVariable* prop)
{
	if (m_defs->showDebugProps)
	{
		if (m_private->debugPage)
		{
			m_private->debugPage->addVariable(prop);
			m_private->debugPage->show(true);
		}
	}
}




void gkEngine::removeDebugProperty(gkVariable* prop)
{
	if (m_defs->showDebugProps)
	{
		if (m_private->debugPage)
			m_private->debugPage->removeVariable(prop);
	}
}



gkScalar gkEngine::getStepRate(void)
{
	return gkScalar(1.0) / m_tickRate;
}



gkScalar gkEngine::getTickRate(void)
{
	return m_tickRate;
}




bool gkEngine::hasActiveScene(void)
{
	GK_ASSERT(m_private);
	return m_private->curScene != 0;
}

void gkEngine::registerActiveScene(gkScene* scene)
{
	GK_ASSERT(m_private && scene);
	if (m_private->scenes.find(scene) == UT_NPOS)
	{
		m_private->scenes.push_back(scene);
		if (m_private->curScene == 0)
		{
			m_private->curScene = scene;
#ifndef BUILD_OGRE18
			scene->getManager()->addRenderQueueListener(m_private->overlaySystem);
#endif
		}
	}
}

void gkEngine::unregisterActiveScene(gkScene* scene)
{
	GK_ASSERT(m_private && scene);
	m_private->scenes.erase(m_private->scenes.find(scene));
	if (m_private->curScene == scene)
	{
		if (m_private->scenes.size()>0)
		{
			m_private->curScene = m_private->scenes.at(0);

#ifndef BUILD_OGRE18
			m_private->curScene->getManager()->addRenderQueueListener(m_private->overlaySystem);
#endif
		}
		else
			m_private->curScene = 0;

	}
}



gkScene* gkEngine::getActiveScene(void)
{
	GK_ASSERT(m_private);
	return m_private->curScene;
}




void gkEngine::addListener(gkEngine::Listener* listener)
{
	m_listeners.push_back(listener);
}

void gkEngine::setScriptListener(gkEngine::Listener* listener)
{
	if (m_scriptListenerSet)
	{
		m_scriptListenerSet = true;
		addListener(listener);
		return;
	}

	//gkLogger::write("You did try to add another gsengine as listener! this is not possible!");
}



void gkEngine::removeListener(gkEngine::Listener* listener)
{
	if (m_listeners.find(listener)!=UT_NPOS)
		m_listeners.erase(listener);
}


void gkEngine::run(void)
{
	if (!initializeStepLoop()) return;

	while (stepOneFrame());

	finalizeStepLoop();

}

bool gkEngine::initializeStepLoop(void)
{
	// init main game loop

	GK_ASSERT(m_private);
	if (m_private->scenes.empty())
	{
		gkLogMessage("Engine: Can't run with out a registered scene. exiting\n");
		return false;
	}

	gkWindowSystem* sys = m_private->windowsystem;
	if (!sys)
	{
		gkLogMessage("Engine: Can't run with out a window system. exiting\n");
		return false;
	}


	// setup timer
	m_private->root->clearEventTimes();
	m_private->root->getRenderSystem()->_initRenderTargets();
	m_private->root->addFrameListener(m_private);
	m_private->reset();

	m_running = true;

	return true;
}


bool gkEngine::stepOneFrame(void)
{
	m_private->curTime = m_private->timer->getTimeMilliseconds();
	//gkLogger::write("tick22", true);
	if (m_requestStep)
	{
		m_requestStep=false;
		m_doStep = true;
	}
	//gkLogger::write("tick222", true);

	gkWindowSystem* sys = m_private->windowsystem;
	//gkLogger::write("tick2222", true);
	sys->process();
	//gkLogger::write("tick22222", true);

	if (!m_private->root->renderOneFrame())
		return false;

	//gkLogger::write("tick222222", true);
	m_doStep = false;
	//gkLogger::write("tick2222222", true);

	return !sys->exitRequest();
}


void gkEngine::finalizeStepLoop(void)
{
#ifdef OGREKIT_COMPILE_LIBROCKET
	if (gkGUIManager::getSingletonPtr())
		delete gkGUIManager::getSingletonPtr();
#endif
	m_private->root->removeFrameListener(m_private);
	m_running = false;
}

unsigned long gkEngine::getCurTime()
{
	return m_private->curTime;
}

bool gkOgreEnginePrivate::frameStarted(const Ogre::FrameEvent& evt)
{
	gkStats::getSingleton().startClock();

	return true;
}



bool gkOgreEnginePrivate::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	gkStats::getSingleton().stopRenderClock();

	if (!scenes.empty())
		tick();

	// restart the clock to measure time for swapping buffer and updatind scenemanager LOD
	gkStats::getSingleton().startClock();

	return !scenes.empty();
}




bool gkOgreEnginePrivate::frameEnded(const Ogre::FrameEvent& evt)
{
	gkStats::getSingleton().stopBufSwapLodClock();
	gkStats::getSingleton().nextFrame();

	return true;
}




void gkOgreEnginePrivate::beginTickImpl(void)
{
	GK_ASSERT(!scenes.empty());

	//gkLogger::write("begin-tick",true);

	gkSceneArray::Iterator iter(scenes);
	while (iter.hasMoreElements())
	{
		//gkLogger::write("begin-frame",true);
		iter.getNext()->beginFrame();	
	}
}




void gkOgreEnginePrivate::endTickImpl(void)
{
	if (debugPage && debugPage->isShown())
		debugPage->draw();

	if (debugFps && debugFps->isShown())
		debugFps->draw();


}




void gkOgreEnginePrivate::tickImpl(gkScalar dt)
{
	// Proccess one full game tick
	GK_ASSERT(windowsystem && !scenes.empty() && engine);


	// dispatch inputs
	windowsystem->dispatch();

	// process event-listeners on queued events
	gkEventManager::Get()->tick();
	// update main scene
	gkSceneArray::Iterator siter1(scenes);
	while (siter1.hasMoreElements())
	{
		gkScene* scene = siter1.getNext();
		curScene = scene;
		scene->update(dt);
	}

	//gkLogger::write("a",true);

	// update callbacks
	utArrayIterator<gkEngine::Listeners> iter(engine->m_listeners);
	while (iter.hasMoreElements())
		iter.getNext()->tick(dt);

	//gkLogger::write("b",true);


	#ifdef OGREKIT_NETWORK_ENET_SUPPORT
			if (gkNetworkClient::getSingletonPtr()){
					gkNetworkClient::getSingleton().poll(0);
			}
	#endif

	//gkLogger::write("c",true);


	gkSceneArray::Iterator siter2(scenes);
	//gkLogger::write("d",true);

	while (siter2.hasMoreElements())
		siter2.getNext()->applyConstraints();

	//gkLogger::write("e",true);
	gkLuaManager::getSingleton().executePreparedLuaEvents();
	//gkLogger::write("f",true);

	gkGameObjectManager::getSingleton().postProcessQueue();
	//gkLogger::write("g",true);
	gkSceneManager::getSingleton().postProcessQueue();
	//gkLogger::write("h",true);

//	const IEventDataPtr tick(new Event_Tick(dt));
//	safeQueueEvent(tick);


}

void gkEngine::handleMessage(gkMessageManager::Message* message)
{
#ifdef OGREKIT_OPENAL_SOUND
	if (message->m_subject==GK_MSG_INTERNAL_GAME_VOLUP)
	{
		float gVol = gkSoundManager::getSingleton().getGlobalVolume();

		gVol += 0.1f;

		if (gVol > 1.0f)
			gVol = 1.0f;

		_LOGI_("VOLUP to %f",gVol)
		gkSoundManager::getSingleton().setGlobalVolume(gVol);
	}
	else if (message->m_subject==GK_MSG_INTERNAL_GAME_VOLDOWN)
	{
		float gVol = gkSoundManager::getSingleton().getGlobalVolume();

		gVol -= 0.1f;

		if (gVol < 0.0f)
			gVol = 0.0f;

		_LOGI_("VOLDOWN to %f",gVol)
		gkSoundManager::getSingleton().setGlobalVolume(gVol);
	}
#endif
}

btClock* gkEngine::getTickClock() { return m_private->getClock();}


UT_IMPLEMENT_SINGLETON(gkEngine);
