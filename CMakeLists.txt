# ---------------------------------------------------------
cmake_minimum_required(VERSION 2.6)

#SET(CMAKE_SUPPRESS_REGENERATION  1)
#SET(CMAKE_USE_RELATIVE_PATHS 1)
#SET(INTERNAL_CREATE_DISTRIBUTABLE_MSVC_PROJECTFILES 1)

project(OGREKIT)
SET(OGREKIT 1.0)
set(OGRE_STATIC TRUE)

if(NOT MINGW AND NOT BLACKBERRY)
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
    CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
    if(COMPILER_SUPPORTS_CXX11)
    	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    	set(CPP11 TRUE)
    	MESSAGE(STATUS "c++11: -std=c++11")
    elseif(COMPILER_SUPPORTS_CXX0X)
    	set(CPP11 TRUE)
    	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    	MESSAGE(STATUS "c++11: -std=c++0x")
    else()
            message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
    endif()
endif()

if (EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_PTHREADS=1  -s TOTAL_MEMORY=1536870912")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s USE_PTHREADS=1  -s TOTAL_MEMORY=1536870912")
endif()

if(COMMAND cmake_policy)
   cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

if (MSVC)
	add_definitions( -D_CRT_SECURE_NO_WARNINGS )
	add_definitions( -D_CRT_SECURE_NO_DEPRECATE )
	add_definitions( -DNOMINMAX)
endif()


if (OGREKIT_BUILD_IPHONE)    
	add_definitions(-DARM_NEON_GCC_COMPATIBILITY=1)
endif()

# ---------------------------------------------------------
 set(GAMEKIT_GRAPHIC_BACKEND 1 CACHE STRING 
        "Select the graphic backend for GameKit. Possible values:
                 1 - Ogre 1.9a
        "
        )

if (GAMEKIT_GRAPHIC_BACKEND EQUAL 1)
        set(GAMEKIT_GRAPHIC_BACKEND_DIR Ogre-1.9a)
else()
        message(FATAL_ERROR "Invalid graphics backend or not supported yet.")
endif()

set(GAMEKIT_ADDITIONAL_DEP_FOLDER "${CMAKE_SOURCE_DIR}/../gamekit-pendencies" CACHE PATH "The path to gamekit-pendencies")
set(RUNTIME_BLENDERNODES "${CMAKE_SOURCE_DIR}/BNodes/out-src" CACHE PATH "The path to blendernodes main-folder")
set(PLAYGROUND_PATH "" CACHE PATH "The path to blendernodes main-folder" )
option(PLAYGROUND_EXPORT   "Copy new binary to runtimes" ON)

message(STATUS "OGRE CMAKE:${OGREKIT_SOURCE_DIR}/${GAMEKIT_GRAPHIC_BACKEND_DIR}/CMake")

set (NEWOGRE_PATH "${GAMEKIT_ADDITIONAL_DEP_FOLDER}/ogre-git")

message(STATUS "__NEW OGRE-CMAKE: ${NEWOGRE_PATH}")

set(CMAKE_MODULE_PATH
    ${OGREKIT_SOURCE_DIR}/CMake
    ${OGREKIT_SOURCE_DIR}/CMake/Utils
    ${OGREKIT_SOURCE_DIR}/CMake/Packages
    ${OGREKIT_SOURCE_DIR}/CMake/Templates 
    ${NEWOGRE_PATH}/CMake
    ${NEWOGRE_PATH}/CMake/Utils
    ${NEWOGRE_PATH}/CMake/Packages
    ${NEWOGRE_PATH}/CMake/Templates

)

message(STATUS "_____Module PATH: ${CMAKE_MODULE_PATH}")

include(FindPkgMacros)
include(CMakeDependentOption)
include(CheckCXXCompilerFlag)
include(ConfigOgreKit)
include(ConfigGameKit)
include(ConfigIPhone)
include(OgreAddTargets)
include(Ogrekit)

configure_gamekit(${OGREKIT_SOURCE_DIR}/Dependencies/Source)

message(STATUS "-----------------------------------------------------------------------------")
message(STATUS "-- Graphic backend is " ${GAMEKIT_GRAPHIC_BACKEND_DIR})
message(STATUS "-----------------------------------------------------------------------------")

configure_ogrekit(${OGREKIT_SOURCE_DIR} ${NEWOGRE_PATH})


set(OGREKIT_BULLET_LIBS BulletDynamics BulletCollision  LinearMath)	
set(OGREKIT_BULLET_INCLUDE ${OGREKIT_SOURCE_DIR}/bullet/src)
if (OGREKIT_COMPILE_SOFTBODY)
	list(APPEND OGREKIT_BULLET_LIBS BulletSoftBody)
endif()

if (NOT APPLE AND OGREKIT_COMPILE_WXWIDGETS)
	include(wxSetup)
	configure_wx_build(${OGREKIT_SOURCE_DIR})
	setup_wx()
endif()


#if (APPLE)
#    if (${XCODE_VERSION} STRGREATER "4.0.0")
#        set(CMAKE_XCODE_ATTRIBUTE_GCC_VERSION "4.2")
#    endif()
#endif()


IF(MSVC)

	#We statically link to reduce dependancies
	FOREACH(flag_var    CMAKE_CXX_FLAGS 
	                    CMAKE_CXX_FLAGS_DEBUG 
	                    CMAKE_CXX_FLAGS_RELEASE 
	                    CMAKE_CXX_FLAGS_MINSIZEREL 
	                    CMAKE_CXX_FLAGS_RELWITHDEBINFO
	                    CMAKE_C_FLAGS_DEBUG
	                    CMAKE_C_FLAGS_RELEASE
	                    CMAKE_C_FLAGS_MINSIZEREL 
	                    CMAKE_C_FLAGS_RELWITHDEBINFO
	                    )
		IF(${flag_var} MATCHES "/MD")
			STRING(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
		ENDIF(${flag_var} MATCHES "/MD")
		IF(${flag_var} MATCHES "/MDd")
			STRING(REGEX REPLACE "/MDd" "/MTd" ${flag_var} "${${flag_var}}")
		ENDIF(${flag_var} MATCHES "/MDd")
	ENDFOREACH(flag_var)

ENDIF(MSVC)


# ---------------------------------------------------------
# Common base paths
set(OGREKIT_INCLUDE ${OGREKIT_OGRE_INCLUDE}
                    ${OGREKIT_SOURCE_DIR}
                    ${OGREKIT_SOURCE_DIR}/Engine/Script/Api
                    ${OGREKIT_SOURCE_DIR}/Engine
                    ${OGREKIT_SOURCE_DIR}/Engine/AI
                    ${OGREKIT_SOURCE_DIR}/GUI
                    ${OGREKIT_BULLET_INCLUDE}
                    ${OGREKIT_DEP_INCLUDE}
                    )

set(OGREKIT_LIB OgreKitCore ${OGREKIT_OGRE_LIBS})


subdirs(bullet)
subdirs(Dependencies)
subdirs(Ogre-1.9a)
MESSAGE(STATUS "BEFORE ENGINE DEPS:${OGREKIT_DEP_INCLUDE}")
subdirs(Engine)

if (NOT OGREKIT_USE_BPARSE OR OGREKIT_USE_FILETOOLS)
	subdirs(${OGREKIT_SOURCE_DIR}/Tools/FileTools)
else()
endif()

if (OGREKIT_COMPILE_WXWIDGETS)
	subdirs(${OGREKIT_SOURCE_DIR}/${WX_VERSION})
endif()

# process addons
subdirs(Addons)
# process samples
subdirs(Samples)

if (OGREKIT_UPDATE_CPP_DOCS)
	message(STATUS "Update CPP")
	subdirs(Docs/CppAPI)
endif()

if (OGREKIT_UPDATE_LUA_DOCS)
	subdirs(Docs/LuaAPI)
endif()

if (OGREKIT_UPDATE_FBT_DOCS)
	subdirs(Docs/FbtAPI)
endif()

if (OGREKIT_ENABLE_UNITTESTS)
	subdirs(UnitTests)
endif()
