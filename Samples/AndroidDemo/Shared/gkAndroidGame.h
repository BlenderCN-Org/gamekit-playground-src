/*
 * AndroidGame.h
 *
 *  Created on: Feb 28, 2013
 *      Author: ttrocha
 */

#ifndef ANDROIDGAME_H_
#define ANDROIDGAME_H_

#include "gkString.h"
#include "gkUserDefs.h"
#include "gkCoreApplication.h"
#include "gkMessageManager.h"
#include "gkMathUtils.h"
#include "android/AndroidInputManager.h"
#include <jni.h>
#include <android/asset_manager.h>
#include "Loaders/Blender2/gkBlendLoader.h"
#include "Loaders/Blender2/gkBlendFile.h"

class gkAndroidGame : public gkCoreApplication, public gkMessageManager::GenericMessageListener
{
public:
	gkAndroidGame(const gkString& initialBlend, JavaVM* jvm,AAssetManager* mgr, const gkString& sdcardBase="/sdcard");
	virtual ~gkAndroidGame();
	// gkCoreApplication
	bool setup(void);
	// gkMessageManager-listener
	void handleMessage(gkMessageManager::Message* message);

	JNIEnv* getEnv()
	{
	    JNIEnv* env = NULL;
	    if (mJVM) (mJVM)->GetEnv((void**)&env, JNI_VERSION_1_4);
	    return env;
	}


private:
	const gkString 	m_initialBlendName;
	gkBlendFile* 	m_initialBlend;
	JavaVM* mJVM;
	AAssetManager* mAssetManager;
	gkString mSDCardPath;
	jmethodID mFireString;
	jmethodID mGetPlatform;
	jclass ANDROID_MAIN;
};

#endif /* ANDROIDGAME_H_ */
