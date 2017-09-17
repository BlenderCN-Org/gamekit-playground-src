/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2012 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#include <jni.h>
#include <EGL/egl.h>
#include <android/api-level.h>
#include <android/native_window_jni.h>
#include "OgrePlatform.h"
#include "OgreRoot.h"
#include "OgreRenderWindow.h"
#include "Android/OgreAndroidEGLWindow.h"

//#include "OgreOctreePlugin.h"
//#include "OgreParticleFXPlugin.h"
#include "OgreConfigFile.h"
#include "gkLogger.h"
#include "gkEngine.h"
#include "gkWindowSystem.h"
#include "gkWindow.h"
#include "OgreStringConverter.h"
#include "gkCoreApplication.h"
#include "gkAndroidGame.h"
#include "gkViewport.h"
#include "OgreViewport.h"
#include "Sound/gkSoundManager.h"
#include "gkValue.h"

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <Events/gkEvents.h>
#include <gkEventManager.h>
#include <gkPath.h>

#include "Android/OgreAPKFileSystemArchive.h"
#include "Android/OgreAPKZipArchive.h"
#include "OgreArchiveManager.h"
//#include "../../../Ogre-1.9a/OgreMain/include/Android/OgreAPKFileSystemArchive.h"
//#include "../../../Ogre-1.9a/OgreMain/include/Android/OgreAPKZipArchive.h"
#include "OgreAPKFileSystemArchive.h"
#include "OgreAPKZipArchive.h"

#include "gkVariable.h"


using namespace Ogre;

static bool gInit = false;
static Ogre::RenderWindow* gRenderWnd = NULL;
static JavaVM* gVM = NULL;
static ANativeWindow* nativeWnd;
static AAssetManager* assetManager;
static OIS::AndroidInputManager* oisinput=NULL;
static gkAndroidGame* m_game = NULL;

static IEventDataPtr msgEventAccel;
static IEventDataPtr msgEventOrient;

extern "C" 
{
	JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) 
	{
        _LOGI_("JNI_ONLOAD")
		JNIEnv *env;
        jclass k;
        jint r;

		gVM = vm;
        r = vm->GetEnv ((void **) &env, JNI_VERSION_1_4);
        k = env->FindClass ("org/gamekit/jni/OgreActivityJNI");

		return JNI_VERSION_1_4;
	}

	JNIEXPORT void JNICALL 	Java_org_gamekit_jni_OgreActivityJNI_create(JNIEnv* env, jclass clazz, jobject jassetManager,jstring jsdcardPath)
	{
        _LOGI_("JNI_CREATE")
        if(gInit)
			return;
		_LOG_FOOT_;

	    assetManager = AAssetManager_fromJava(env, jassetManager);

		const char* str4 = env->GetStringUTFChars(jsdcardPath, 0);
		gkString sdcardPath = str4;
		env->ReleaseStringUTFChars(jsdcardPath,str4);

		gkPath::setAssetManager(assetManager);
		gkPath::setSDCardPath(sdcardPath);

		m_game = new gkAndroidGame("game.dat",gVM,assetManager,sdcardPath);

		gInit = true;

	}
	
	JNIEXPORT void JNICALL Java_org_gamekit_jni_OgreActivityJNI_destroy(JNIEnv * env, jobject obj)
	{
        _LOGI_("JNI_destroy")
		if(!gInit)
			return;
                
        gInit = false;

        _LOG_FOOT_;

        gRenderWnd = NULL;

        ANativeWindow_release(nativeWnd);
	}
	

    JNIEXPORT void JNICALL Java_org_gamekit_jni_OgreActivityJNI_initWindow(JNIEnv * env, jobject obj,  jobject surface)
	{
    	_LOGI_("JNI__initWindow")

    	if(surface)
		{
    		_LOG_FOOT_;
			nativeWnd = ANativeWindow_fromSurface(env, surface);

			if (nativeWnd)
			{
	    		_LOG_FOOT_;
				if (!gRenderWnd) 
				{
		    		_LOG_FOOT_;
		    		_LOG_FOOT_;
					Ogre::NameValuePairList opt;
//					opt["externalWindowHandle"] = Ogre::StringConverter::toString((int)nativeWnd);
		    		_LOG_FOOT_;
		    		_LOG_FOOT_;
		    		//m_game->getPrefs().buildStaticGeometry=true;
		    		//m_game->getPrefs().debugFps=true;

		    		m_game->getPrefs().extWinhandle = Ogre::StringConverter::toString((int)nativeWnd);
		    		_LOG_FOOT_;
		    		if (m_game->initialize())
		    		{

			    		gkEngine::getSingleton().initializeStepLoop();
	//					gRenderWnd = Ogre::Root::getSingleton().createRenderWindow("OgreWindow", 800, 480, false, &opt);
						//gkEngine::getSingleton().initializeWindow();
			    		_LOG_FOOT_;
						gRenderWnd = gkWindowSystem::getSingleton().getMainRenderWindow();


						gkWindow* gkWin = gkWindowSystem::getSingleton().getMainWindow();
						oisinput = static_cast<OIS::AndroidInputManager*>(gkWin->getInputManager());
						oisinput->setWindowSize(gkWin->getWidth(),gkWin->getHeight());

						msgEventAccel = IEventDataPtr(new gkMessageEvent("mobile-accel"));
						static_cast<gkMessageEvent*>(msgEventAccel.get())->setData("type",new gkVariable("accel"));

						msgEventOrient = IEventDataPtr(new gkMessageEvent("mobile-orient"));
						static_cast<gkMessageEvent*>(msgEventAccel.get())->setData("type",new gkVariable("orient"));

						gkLogger::write("Window:"+gkToString(gkWin->getWidth())+"x"+gkToString(gkWin->getHeight()),true);


						//gkWin->getViewport(0)->getViewport()->setBackgroundColour(Ogre::ColourValue(0,0,0,1));

						//					if(pSceneMgr == NULL)
	//					{
	//						pSceneMgr = gRoot->createSceneManager(Ogre::ST_GENERIC);
	//						pCamera = pSceneMgr->createCamera("MyCam");

		    		}

				}
				else
				{
		    		_LOG_FOOT_;
					static_cast<Ogre::AndroidEGLWindow*>(gRenderWnd)->_createInternalResources(nativeWnd, NULL);
		    		_LOG_FOOT_;
				}                        
			}
		}
	}
	
    JNIEXPORT void JNICALL Java_org_gamekit_jni_OgreActivityJNI_termWindow(JNIEnv * env, jobject obj)
	{
    	_LOGI_("termWindow");
		if(gRenderWnd)
		{
			static_cast<Ogre::AndroidEGLWindow*>(gRenderWnd)->_destroyInternalResources();
		}
	}
	
	JNIEXPORT void JNICALL Java_org_gamekit_jni_OgreActivityJNI_renderOneFrame(JNIEnv * env, jobject obj)
	{


		if(gRenderWnd != NULL && gRenderWnd->isActive())
		{
	    	//_LOGI_("render");
			try
			{
		    	//_LOG_FOOT_

				if(gVM->AttachCurrentThread(&env, NULL) < 0) 					
					return;
				
		    //	_LOG_FOOT_
				gRenderWnd->windowMovedOrResized();
		    //	_LOG_FOOT_

		    //	_LOG_FOOT_

//	 			gRoot->renderOneFrame();
		    	gkEngine::getSingleton().stepOneFrame();
				
				//gVM->DetachCurrentThread();				
			}catch(Ogre::RenderingAPIException& ex) {
				_LOG_FOOT_;
				_LOGI_("RenderingAPIException");
			}
		}
	}

	JNIEXPORT jboolean JNICALL Java_org_gamekit_jni_OgreActivityJNI_multitouchEvent
	  (JNIEnv * env, jobject jobj, jint action, jint numInputs, jfloatArray jdata, jint dataStride)
	{
		if (gRenderWnd != NULL && gRenderWnd->isActive())
		{
			jfloat* data = env->GetFloatArrayElements(jdata,0);
			for (int i=0;i<numInputs;i++)
			{
				float x = data[i*dataStride];
				float y = data[i*dataStride+1];
				float pointerId = (int)data[i*dataStride+2];
				float size = data[i*dataStride+3];
				float pressure = data[i*dataStride+4];
//				oisinput->injectTouch(action,x,y);

				oisinput->injectMultiTouch(action,pointerId,x,y,size,pressure);
			}
			env->ReleaseFloatArrayElements(jdata,data,0);
		}
		return true;
	}

//    public native static boolean keyEvent(int action, int unicodeChar, int keyCode, KeyEvent event);
	JNIEXPORT jboolean JNICALL Java_org_gamekit_jni_OgreActivityJNI_keyEvent
	  (JNIEnv * env, jobject jobj, jint action, jint modifiers, jint keyCode)
	{
		if (gRenderWnd != NULL && gRenderWnd->isActive())
		{
			_LOGI_("a-inject: act:%i key:%i uni:%i",action,keyCode,modifiers);
			oisinput->injectKey(action,modifiers,keyCode);
		}
		return true;
	}


	/*
	 * Class:     org_gamekit_jni_OgreActivityJNI
	 * Method:    setOffset
	 * Signature: (II)V
	 */
	JNIEXPORT void JNICALL Java_org_gamekit_jni_OgreActivityJNI_setOffset
	  (JNIEnv* env, jclass clazz, jint x, jint y)
	{
		if (oisinput) {
			oisinput->setOffsets(x,y);
		}
	}

	/*
	 * Class:     org_gamekit_jni_OgreActivityJNI
	 * Method:    pause
	 * Signature: ()V
	 */
	JNIEXPORT void JNICALL Java_org_gamekit_jni_OgreActivityJNI_pause
	  (JNIEnv *, jclass){
		if (gkSoundManager::getSingletonPtr())
			gkSoundManager::getSingleton().stop();
		if (gkMessageManager::getSingletonPtr())
			gkMessageManager::getSingleton().sendMessage(GK_MSG_GROUP_INTERNAL,"",GK_MSG_INTERNAL_GAME_PAUSE,"");
	}

	/*
	 * Class:     org_gamekit_jni_OgreActivityJNI
	 * Method:    resume
	 * Signature: ()V
	 */
	JNIEXPORT void JNICALL Java_org_gamekit_jni_OgreActivityJNI_resume
	  (JNIEnv *, jclass){
		if (gkSoundManager::getSingletonPtr())
			gkSoundManager::getSingleton().restart();
		if (gkMessageManager::getSingletonPtr())
			gkMessageManager::getSingleton().sendMessage(GK_MSG_GROUP_INTERNAL,"",GK_MSG_INTERNAL_GAME_RESUME,"");
	}

	JNIEXPORT void JNICALL Java_org_gamekit_jni_OgreActivityJNI_nativeCommitText
		(JNIEnv *env, jclass clazz, jstring jtext, jint pos){
		const char* str = env->GetStringUTFChars(jtext, 0);
		gkString text = str;
		env->ReleaseStringUTFChars(jtext,str);
		gkLogger::write("nativeCommit:"+text);
	}

	/*
	 * Class:     org_gamekit_jni_OgreActivityJNI
	 * Method:    sendMessage
	 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V
	 */
	JNIEXPORT void JNICALL Java_org_gamekit_jni_OgreActivityJNI_sendMessage
	  (JNIEnv *env, jclass clazz, jstring jfrom, jstring jto, jstring jsubject, jstring jbody){
		const char* str = env->GetStringUTFChars(jfrom, 0);
		gkString from = str;
		env->ReleaseStringUTFChars(jfrom,str);

		const char* str2 = env->GetStringUTFChars(jto, 0);
		gkString to = str2;
		env->ReleaseStringUTFChars(jto,str2);

		const char* str3 = env->GetStringUTFChars(jsubject, 0);
		gkString subject = str3;
		env->ReleaseStringUTFChars(jsubject,str3);

		const char* str4 = env->GetStringUTFChars(jbody, 0);
		gkString body =  str4;
		env->ReleaseStringUTFChars(jbody,str4);


		if (gkMessageManager::getSingletonPtr()){
			gkMessageManager::getSingletonPtr()->sendMessage(from,to,subject,body);
		}
	}


	JNIEXPORT void JNICALL Java_org_gamekit_jni_OgreActivityJNI_onControllerJoystickMove(JNIEnv* env, jclass clazz, jint player, jint joystick, jfloat x, jfloat y)
	{
	    oisinput->injectJoystick(player,joystick,x,y);
		gkLogger::write("ANDROID JOY MOVE",true);
	}

	JNIEXPORT void JNICALL Java_org_gamekit_jni_OgreActivityJNI_onControllerButtonDown(JNIEnv* env, jclass clazz, jint player, jint button)
	{
	    oisinput->injectJoystickButton(player,button,true);
	    gkLogger::write("ANDROID JOY DOWN",true);
	}

	JNIEXPORT void JNICALL Java_org_gamekit_jni_OgreActivityJNI_onControllerButtonUp(JNIEnv* env, jclass clazz, jint player, jint button)
	{
	    oisinput->injectJoystickButton(player,button,false);
	    gkLogger::write("ANDROID JOY UP",true);
	}



//    public native static void sendAcceleratorVector(float x,float y,float z);
	JNIEXPORT void JNICALL Java_org_gamekit_jni_OgreActivityJNI_sendAcceleratorVector(JNIEnv* env, jclass clazz, jfloat x, jfloat y, jfloat z)
	{
//		gkLogger::write("ANDROID accel:"+gkToString(x)+":"+gkToString(y)+":"+gkToString(z),true);
		gkVector3 accelVec(x,y,z);
		gkString type(msgEventAccel.get()->getEventType().str());
		gkVariable var;
		var.setValue(accelVec);
		static_cast<gkMessageEvent*>(msgEventAccel.get())->setData("value",&var);
		safeQueueEvent(msgEventAccel);
	}


//    public native static void sendOrientationVector(float x,float y,float z);
	JNIEXPORT void JNICALL Java_org_gamekit_jni_OgreActivityJNI_sendOrientationVector(JNIEnv* env, jclass clazz, jfloat x, jfloat y, jfloat z)
	{
//		gkLogger::write("ANDROID orient:"+gkToString(x)+":"+gkToString(y)+":"+gkToString(z),true);
		gkVector3 orientVec(x,y,z);
		static_cast<gkMessageEvent*>(msgEventOrient.get())->setData("value",new gkVariable(&orientVec));
		safeQueueEvent(msgEventOrient);
	}

};
