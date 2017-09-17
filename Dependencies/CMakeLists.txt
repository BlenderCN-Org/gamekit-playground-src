# ---------------------------------------------------------
cmake_minimum_required(VERSION 2.6)

project(Dependencies)
SET(Dependencies 1.0)

subdirs(Source/GameKit/Utils)
subdirs(Source/GameKit/AnimKit)

if (OGREKIT_USE_BPARSE)
    include_directories( GameKit/Utils )
	subdirs(Source/Blender25)
endif()

if (OGREKIT_HEADER_GENERATOR)
	subdirs(Source/Blender25/Headers)
endif()

include(NoWarn)
subdirs(Source/OIS)
subdirs(Source/FreeType)


if (OGREKIT_COMPILE_OPENSTEER)
	subdirs(Source/OpenSteer)
endif()

if (OGREKIT_COMPILE_RECAST)
#	subdirs(Source/Recast)
#	subdirs(Source/Detour)
	subdirs(Source/RecastDetour)
#subdirs(Source/masa_recast)
endif()

if (OGREKIT_USE_STATIC_FREEIMAGE)
	subdirs(Source/FreeImage)
endif()

if (NOT OGREKIT_DISABLE_ZIP)
	subdirs(Source/ZZipLib)
endif()

if (OGREKIT_USE_LUA)
	if ("${OGREKIT_EXTERNAL_LUA_LIB}" STREQUAL "")
		subdirs(Source/Lua)
	endif()
endif()

if (OGREKIT_OPENAL_SOUND)
	subdirs(Source/Codecs)
	
	if (WIN32)
		subdirs(Source/OpenAL)
	endif()
endif()

if (OGREKIT_COMPILE_TINYXML)
    subdirs(Source/TinyXml)
endif()

if (OGREKIT_COMPILE_LIBROCKET)
	subdirs(Source/libRocket/Build)
endif()

if (OGREKIT_NETWORK_ENET_SUPPORT)
	subdirs(Source/Enet)
endif()

subdirs(Source/CppTweener)

if (OGREKIT_COMPILE_GUI3D)
	subdirs(Source/GUI3D)
endif()


#if (OGREKIT_BUILD_MOBILE AND OGRE_CONFIG_ENABLE_GLES2_GLSL_OPTIMISER)
if (OGRE_CONFIG_ENABLE_GLES2_GLSL_OPTIMISER)
    subdirs(Source/glsl-optimiser)
endif()

# on android use the precompiled libcurl (see also Docs/readme.libcurl)
if (OGREKIT_COMPILE_LIBCURL AND NOT OGREKIT_BUILD_MOBILE 
	AND NOT MINGW
)
    subdirs(Source/libcurl)
else()
    MESSAGE(STATUS "1895 NO CURL COMPILATION!")
endif()

if (OGREKIT_COMPILE_PROTOCOLBUFFERS)
    subdirs(Source/ProtocolBuffers)
endif()

if (OGREKIT_COMPILE_THRIFT)
    subdirs(Source/thrift)
endif()

if (OGREKIT_COMPILE_OPENSSL)
    message(STATUS "I DO COMPILE OPENSSL!")
    subdirs(Source/openssl)
endif()
