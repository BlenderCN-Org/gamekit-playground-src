ANDROID_STARTER_PATH := $(call my-dir)/../../Shared
LIB_PATH := /home/ttrocha/_dev/extprojects/gamekit/libs/armeabi-v7a
OGRE_PATH := /home/ttrocha/_dev/extprojects/_build/gamekit-android/Bin/lib
OPENAL_PATH := /home/ttrocha/_dev/extprojects/gamekit/Dependencies/android/openal/armeabi-v7a

# Started to compiling it with the code (see readme.curl)
LIBCURL_PATH := /home/ttrocha/_dev/extprojects/gamekit/Dependencies/android/libcurl/arm
OPENSSL_PATH := /home/ttrocha/_dev/extprojects/gamekit/Dependencies/android/openssl

LOCAL_PATH := $(ANDROID_STARTER_PATH)
include $(CLEAR_VARS)

LOCAL_MODULE    := ogrekit
#LOCAL_SRC_FILES := AndroidLogListener.cpp Main.cpp
LOCAL_SRC_FILES := gkAndroidGame.cpp OgreActivityJNI.cpp 

LOCAL_C_INCLUDES := /home/ttrocha/_dev/extprojects/gamekit/Ogre-1.9a/OgreMain/include
LOCAL_C_INCLUDES += /home/ttrocha/_dev/extprojects/gamekit/Engine

LOCAL_LDLIBS    :=  -llog -lGLESv2 -lEGL -landroid -lz 
#-lstdc++ -lsupc++
LOCAL_CFLAGS    := -D__ANDROID__ -Wno-psabi  

LOCAL_CFLAGS    += -I/home/ttrocha/_dev/extprojects/_build/gamekit-android/Engine -I/home/ttrocha/_dev/extprojects/gamekit/Ogre-1.9a/OgreMain/include 
LOCAL_CFLAGS    += -I/home/ttrocha/_dev/extprojects/gamekit/Ogre-1.9a/Components/Overlay/include -I/home/ttrocha/_dev/extprojects/_build/gamekit-android/include 
LOCAL_CFLAGS    += -I/home/ttrocha/_dev/extprojects/gamekit/Ogre-1.9a/OgreMain/include/GLX -I/home/ttrocha/_dev/extprojects/gamekit/Ogre-1.9a/Components/RTShaderSystem/include 
LOCAL_CFLAGS    += -I/home/ttrocha/_dev/extprojects/gamekit/Engine/AI -I/home/ttrocha/_dev/extprojects/gamekit/GUI -I/home/ttrocha/_dev/extprojects/gamekit/bullet/src 
LOCAL_CFLAGS    += -I/home/ttrocha/_dev/extprojects/gamekit/Dependencies/Source/FreeType/include -I/home/ttrocha/_dev/extprojects/gamekit/Dependencies/Source/FreeImage/ZLib 
LOCAL_CFLAGS    += -I/home/ttrocha/_dev/extprojects/gamekit/Dependencies/Source/OIS/include -I/home/ttrocha/_dev/extprojects/gamekit/Dependencies/Source/GameKit/Utils 
LOCAL_CFLAGS    += -I/home/ttrocha/_dev/extprojects/gamekit/Dependencies/Source/GameKit/AnimKit -I/home/ttrocha/_dev/extprojects/gamekit/Dependencies/Source/CppTweener/include 
LOCAL_CFLAGS    += -I/home/ttrocha/_dev/extprojects/gamekit/Dependencies/Source/GUI3D/Gorilla -I/home/ttrocha/_dev/extprojects/gamekit/Dependencies/Source/GUI3D/src 
LOCAL_CFLAGS    += -I/home/ttrocha/_dev/extprojects/gamekit/Tools/FileTools/File -I/home/ttrocha/_dev/extprojects/gamekit/Tools/FileTools/FileFormats/Blend 
LOCAL_CFLAGS    += -I/home/ttrocha/_dev/extprojects/gamekit/Dependencies/Source/libRocket/Include -I/home/ttrocha/_dev/extprojects/gamekit/Dependencies/Source/libRocket/Source 
LOCAL_CFLAGS    += -I/home/ttrocha/_dev/extprojects/gamekit/Dependencies/Source/Lua/lua -I/home/ttrocha/_dev/extprojects/gamekit/Dependencies/Source/ZZipLib
# new
LOCAL_CFLAGS    += -I/home/ttrocha/_dev/extprojects/gamekit/Ogre-1.9a/RenderSystems/GLES2/include -I/home/ttrocha/_dev/extprojects/gamekit/Ogre-1.9a/RenderSystems/GLES2/include/EGL
LOCAL_CFLAGS    += -I/home/ttrocha/_dev/extprojects/gamekit/Ogre-1.9a/PlugIns/ParticleFX/include -I/home/ttrocha/_dev/extprojects/gamekit/Dependencies/Source/OpenAL
LOCAL_CFLAGS    += -I/home/ttrocha/_dev/extprojects/gamekit/Dependencies/Source/glsl-optimiser/src/glsl
LOCAL_CFLAGS    += -I/home/ttrocha/_dev/extprojects/gamekit/Dependencies/Source/thrift/src -I/home/ttrocha/_dev/extprojects/gamekit/Dependencies/Source/thrift/config/unix
LOCAL_CFLAGS    += -I/home/ttrocha/_dev/extprojects/gamekit/Dependencies/android/boost/include/boost-1_53
LOCAL_CFLAGS    += -I/home/ttrocha/_dev/extprojects/gamekit/Dependencies/android/include

# recast
LOCAL_CFLAGS    += -I/home/ttrocha/_dev/extprojects/gamekit/Dependencies/Source/OpenSteer/include -I/home/ttrocha/_dev/extprojects/gamekit/Dependencies/Source/OpenSteer/include -I/home/ttrocha/_dev/extprojects/gamekit/Dependencies/Source/Recast/Include



#LOCAL_STATIC_LIBRARIES := OgreKitCore;OgreMain;OgreOverlay;OgreProcedural;FreeImage;freetype;GameKitUtils;OIS;ZLib;AnimKit;cpptweener;curl;GUI3D;fbtFile;bfBlend;OgreRTShaderSystem;/home/ttrocha/_dev/extprojects/gamekit/Dependencies/Source/../android/openal/armeabi-v7a/libopenal.so;OggVorbis;Lua;ZZipLib;Recast;OpenSteer
LOCAL_STATIC_LIBRARIES := OgreKitCore libcurl  ssl crypto OgreProcedural OgreMainStatic OgreOverlayStatic OIS FreeImage freetype GameKitUtils ZLib AnimKit cpptweener 
LOCAL_STATIC_LIBRARIES += GUI3D ZZipLib fbtFile bfBlend OgreRTShaderSystemStatic Lua  BulletDynamics BulletCollision LinearMath 
LOCAL_STATIC_LIBRARIES += Plugin_ParticleFXStatic RenderSystem_GLES2Static OgreMainStatic RocketControls RocketDebugger RocketCore 
LOCAL_STATIC_LIBRARIES += cpufeatures android_native_app_glue OggVorbis  gknet ENET ZZipLib FreeImage 
LOCAL_STATIC_LIBRARIES += OpenSteer Detour Recast

# Thrift-Addon 
#LOCAL_STATIC_LIBRARIES += addonThrift thrift gknet ENET 
LOCAL_CFLAGS    += -I/home/ttrocha/_dev/extprojects/gamekit/Addons/AddonThrift

# Default ADDON
LOCAL_CFLAGS    += -I/home/ttrocha/_dev/extprojects/gamekit/Addons/DefaultAddon
LOCAL_STATIC_LIBRARIES += defaultGameAddon 

#LOCAL_STATIC_LIBRARIES += glsloptimizer glcpp mesa

LOCAL_SHARED_LIBRARIES := openal

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/cpufeatures)
$(call import-module,android/native_app_glue)

LOCAL_PATH := $(LIB_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := OgreKitCore 
LOCAL_SRC_FILES := libOgreKitCore.a 
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_PATH := $(LIB_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := Recast 
LOCAL_SRC_FILES := libRecast.a 
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_PATH := $(LIB_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := Detour
LOCAL_SRC_FILES := libDetour.a 
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_PATH := $(LIB_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := OpenSteer
LOCAL_SRC_FILES := libOpenSteer.a 
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_PATH := $(OGRE_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := OgreMainStatic 
LOCAL_SRC_FILES := libOgreMainStatic.a 
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_PATH := $(OGRE_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := OgreOverlayStatic 
LOCAL_SRC_FILES := libOgreOverlayStatic.a 
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_PATH := $(LIB_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := FreeImage 
LOCAL_SRC_FILES := libFreeImage.a 
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_PATH := $(LIB_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := freetype 
LOCAL_SRC_FILES := libfreetype.a 
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_PATH := $(LIB_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := OIS 
LOCAL_SRC_FILES := libOIS.a 
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_PATH := $(LIB_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := OgreProcedural 
LOCAL_SRC_FILES := libOgreProcedural.a 
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_PATH := $(LIB_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := GameKitUtils 
LOCAL_SRC_FILES := libGameKitUtils.a 
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_PATH := $(LIB_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := ZLib 
LOCAL_SRC_FILES := libZLib.a 
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_PATH := $(LIB_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := AnimKit 
LOCAL_SRC_FILES := libAnimKit.a 
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_PATH := $(LIB_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := cpptweener 
LOCAL_SRC_FILES := libcpptweener.a 
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_PATH := $(LIB_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := GUI3D 
LOCAL_SRC_FILES := libGUI3D.a 
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_PATH := $(LIB_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := fbtFile 
LOCAL_SRC_FILES := libfbtFile.a 
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_PATH := $(LIB_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := bfBlend 
LOCAL_SRC_FILES := libbfBlend.a 
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_PATH := $(OGRE_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := OgreRTShaderSystemStatic 
LOCAL_SRC_FILES := libOgreRTShaderSystemStatic.a 
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_PATH := $(LIB_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := Lua 
LOCAL_SRC_FILES := libLua.a 
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_PATH := $(LIB_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := ZZipLib 
LOCAL_SRC_FILES := libZZipLib.a 
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_PATH := $(LIB_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := BulletDynamics 
LOCAL_SRC_FILES := libBulletDynamics.a 
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_PATH := $(LIB_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := BulletCollision 
LOCAL_SRC_FILES := libBulletCollision.a 
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_PATH := $(LIB_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := LinearMath 
LOCAL_SRC_FILES := libLinearMath.a 
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_PATH := $(OGRE_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := RenderSystem_GLES2Static 
LOCAL_SRC_FILES := libRenderSystem_GLES2Static.a 
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_PATH := $(OGRE_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := Plugin_ParticleFXStatic 
LOCAL_SRC_FILES := libPlugin_ParticleFXStatic.a 
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_PATH := $(LIB_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := RocketControls 
LOCAL_SRC_FILES := libRocketControls.a 
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_PATH := $(LIB_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := RocketDebugger
LOCAL_SRC_FILES := libRocketDebugger.a 
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_PATH := $(LIB_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := RocketCore 
LOCAL_SRC_FILES := libRocketCore.a 
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_PATH := $(LIB_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := OggVorbis 
LOCAL_SRC_FILES := libOggVorbis.a 
include $(PREBUILT_STATIC_LIBRARY)

#LOCAL_PATH := $(LIB_PATH)
#include $(CLEAR_VARS)
#LOCAL_MODULE    := mesa 
#LOCAL_SRC_FILES := libmesa.a 
#include $(PREBUILT_STATIC_LIBRARY)
#
#LOCAL_PATH := $(LIB_PATH)
#include $(CLEAR_VARS)
#LOCAL_MODULE    := glsloptimizer 
#LOCAL_SRC_FILES := libglsl_optimizer.a 
#include $(PREBUILT_STATIC_LIBRARY)
#
#LOCAL_PATH := $(LIB_PATH)
#include $(CLEAR_VARS)
#LOCAL_MODULE    := glcpp 
#LOCAL_SRC_FILES := libglcpp-library.a 
#include $(PREBUILT_STATIC_LIBRARY)

LOCAL_PATH := $(OPENAL_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := openal
LOCAL_SRC_FILES := libopenal.so 
include $(PREBUILT_SHARED_LIBRARY)

LOCAL_PATH := $(GAMEKIT_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := openal
LOCAL_SRC_FILES := libopenal.so 
include $(PREBUILT_SHARED_LIBRARY)


LOCAL_PATH := $(OPENSSL_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := crypto
LOCAL_SRC_FILES := libcrypto.a 
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_PATH := $(OPENSSL_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := ssl
LOCAL_SRC_FILES := libssl.a
include $(PREBUILT_STATIC_LIBRARY)


LOCAL_PATH := $(LIBCURL_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := libcurl 
LOCAL_SRC_FILES := libcurl.a 
include $(PREBUILT_STATIC_LIBRARY)

# addon
LOCAL_PATH := $(LIB_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := defaultGameAddon 
LOCAL_SRC_FILES := libDefaultGameAddon.a 
include $(PREBUILT_STATIC_LIBRARY)

#LOCAL_PATH := $(LIB_PATH)
#include $(CLEAR_VARS)
#LOCAL_MODULE    := thrift 
#LOCAL_SRC_FILES := libthrift.a 
#include $(PREBUILT_STATIC_LIBRARY)

#LOCAL_PATH := $(LIB_PATH)
#include $(CLEAR_VARS)
#LOCAL_MODULE    := addonThrift 
#LOCAL_SRC_FILES := libThriftAddon.a 
#include $(PREBUILT_STATIC_LIBRARY)

LOCAL_PATH := $(LIB_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := ENET 
LOCAL_SRC_FILES := libENET.a 
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_PATH := $(LIB_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := gknet 
LOCAL_SRC_FILES := libgknet.a 
include $(PREBUILT_STATIC_LIBRARY)
