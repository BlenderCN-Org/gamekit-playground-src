/*
 * AndroidGame.cpp
 *
 *  Created on: Feb 28, 2013
 *      Author: ttrocha
 */

#include "gkAndroidGame.h"

#include "Loaders/Blender2/gkBlendLoader.h"
#include "Loaders/Blender2/gkBlendFile.h"
#include "gkUtils.h"
#include "gkPath.h"
#include "gkLogger.h"
#include "gkWindow.h"
#include "gkWindowSystem.h"
#include "gkScene.h"
#include "gkMessageManager.h"
#include "OgreArchiveManager.h"
#include "Android/OgreAPKFileSystemArchive.h"
#include "Android/OgreAPKZipArchive.h"
#include "OgreRoot.h"
#include "OgreResourceGroupManager.h"
#include "Addons.h"

#include "gkGamekit.h"
#include "gkMathNodeImpl.h"
#include "gkNodeTreeNodeImpl.h"

gkAndroidGame::gkAndroidGame(const gkString& initialBlend,JavaVM* jvm,AAssetManager* mgr, const gkString& sdcardBase)
	: m_initialBlendName(initialBlend), mJVM(jvm),mAssetManager(mgr),m_initialBlend(0),mSDCardPath(sdcardBase)
{
	_LOG_FOOT_
	gkPath::setSDCardPath(sdcardBase);
	_LOG_FOOT_

	JNIEnv* env = this->getEnv();
	_LOG_FOOT_

    ANDROID_MAIN = static_cast<jclass>(env->NewGlobalRef((env)->FindClass( "org/gamekit/jni/OgreActivityJNI")));
	_LOG_FOOT_
    if (!ANDROID_MAIN)
    {
    	_LOGI_("COULDNT FIND OgreActivityJNI!!!");
    	return ;
    }
	_LOG_FOOT_
    mFireString = (((env)->GetStaticMethodID(ANDROID_MAIN, "onMessage", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V")));
	_LOG_FOOT_
	//mGetPlatform = (((env)->GetStaticMethodID(ANDROID_MAIN, "getPlatform", "(V)I")));
}

gkAndroidGame::~gkAndroidGame() {
	// TODO Auto-generated destructor stub
	JNIEnv* env = this->getEnv();
	env->DeleteGlobalRef(ANDROID_MAIN);
}


bool gkAndroidGame::setup()
{
	_LOG_FOOT_;




//	gkLogger::enable("ogrkeit",true);
	// try to load the blend
	if (m_initialBlendName.empty()) // check filename
	{
		_LOG_FOOT_
		gkLogger::write("No blend-file specified!!! Abort!!",true);
		return false;
	}

//	Ogre::Root::getSingleton().addResourceLocation("/media/textures", "APKFileSystem", "textures");
//	Ogre::Root::getSingleton().addResourceLocation("/media/programs/GLSLES", "APKFileSystem", "textures");
//	Ogre::Root::getSingleton().addResourceLocation(".", "APKFileSystem", "textures");
//	Ogre::Root::getSingleton().addResourceLocation("/media/scripts", "APKFileSystem", "textures");
//	Ogre::ResourceGroupManager::getSingleton().loadResourceGroup("textures");

	Gamekit_addNodeInstancer();
	gkNodeTreeConverter::addNodeInstancer(new MathNodeInstancer());
	gkNodeTreeConverter::addNodeInstancer(new gkNodeTreeNodeCustomInstancer());
	gkAddonAdder::addAddons();
	gkAddonAdder::setupAddons();

	m_initialBlend = gkBlendLoader::getSingleton().loadFile(m_initialBlendName,gkBlendLoader::LO_ALL_SCENES);

	if (!m_initialBlend)
	{
		gkLogger::write("ERROR: Something went wrong loading blend-file:"+m_initialBlendName+"!!! Abort!!",true);
		return false;
	}

	// retrieve a first scene from the current blend-file
	// try to find a scene called "StartScene" (case-sensitive)
	// if not found take the scene that was active when writing the blend-file
	// retrieve scenes afterward with:
	// gkSceneManager::getSingleton().getByName("StartScene")

	gkScene* m_scene = m_initialBlend->getSceneByName("StartScene");

	if (!m_scene)
		m_scene = m_initialBlend->getMainScene();

	if (!m_scene)
	{
		gkLogger::write("No usable scenes found in blend.\n",true);
		return false;
	}

	// create the scene
	m_scene->createInstance();

	gkWindow* win = gkWindowSystem::getSingleton().getMainWindow();

	gkMessageManager::getSingleton().addListener(this);



	return true;
}

void gkAndroidGame::handleMessage(gkMessageManager::Message* message){

	if (message->m_from == "android")
		return;

	JNIEnv* env = this->getEnv();

//    jstring from = this->JNU_NewStringNative(env,message->m_subject.c_str());
    jstring from = env->NewStringUTF(message->m_from.c_str());
    jstring to = env->NewStringUTF(message->m_to.c_str());
    jstring subject = env->NewStringUTF(message->m_subject.c_str());
    jstring body = env->NewStringUTF(message->m_body.c_str());
    env->CallStaticVoidMethod(ANDROID_MAIN,mFireString, from,to,subject,body);

}
