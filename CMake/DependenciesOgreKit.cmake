#-------------------------------------------------------------------
# This file is part of the CMake build system for OGRE
#     (Object-oriented Graphics Rendering Engine)
# For the latest info, see http://www.ogre3d.org/
#
# The contents of this file are placed in the public domain. Feel
# free to make use of it in any way you like.
#-------------------------------------------------------------------
include(MacroLogFeature)


#######################################################################
# Core dependencies
#######################################################################

# Find X11
if (UNIX AND NOT OGREKIT_BUILD_MOBILE AND NOT APPLE AND NOT EMSCRIPTEN) 
	## TEST THIS
	find_package(X11)
	macro_log_feature(X11_FOUND "X11" "X Window system" "http://www.x.org" TRUE "" "")
	macro_log_feature(X11_Xt_FOUND "Xt" "X Toolkit" "http://www.x.org" TRUE "" "")
	find_library(XAW_LIBRARY NAMES Xaw Xaw7 PATHS ${DEP_LIB_SEARCH_DIR} ${X11_LIB_SEARCH_PATH})
	macro_log_feature(XAW_LIBRARY "Xaw" "X11 Athena widget set" "http://www.x.org" TRUE "" "")
	mark_as_advanced(XAW_LIBRARY)
endif ()

if (OGREKIT_COMPILE_PROTOCOLBUFFERS)
	message(STATUS "Try to locate protocol buffers")
	FIND_PACKAGE(ProtocolBuffers)
	message(STATUS "##-Protoc: ${PROTOBUF_FOUND}")
endif()

if (OGREKIT_BUILD_MOBILE)
	# Find OpenGLES
	find_package(OpenGLES)
	macro_log_feature(OPENGLES_FOUND "OpenGLES" "Support for the OpenGLES render system" "" FALSE "" "")

	# Find OpenGLES2
	find_package(OpenGLES2)
	macro_log_feature(OPENGLES2_FOUND "OpenGLES2" "Support for the OpenGLES2 render system" "" FALSE "" "")


else()
	# Find OpenGL
	find_package(OpenGL)
	macro_log_feature(OPENGL_FOUND "OpenGL" "Support for the OpenGL render system" "http://www.opengl.org/" FALSE "" "")
	
	# Find OpenGLES
	find_package(OpenGLES)
	macro_log_feature(OPENGLES_FOUND "OpenGLES" "Support for the OpenGLES render system" "" FALSE "" "")

	# Find OpenGLES2
	find_package(OpenGLES2)
	macro_log_feature(OPENGLES2_FOUND "OpenGLES2" "Support for the OpenGLES2 render system" "" FALSE "" "")

endif()

if (OGREKIT_USE_OPENSSL AND NOT OGREKIT_COMPILE_OPENSSL AND NOT OPENSSL_FOUND)
	message(STATUS "TRY TO FIND THE SYSTEM OPENSSL")
	find_package(OpenSSL)
endif()

if(OGREKIT_USE_BOOST)
	# Find Boost
	# Prefer static linking in all cases
	if (WIN32 OR APPLE)
		set(Boost_USE_STATIC_LIBS TRUE)
	else ()
		# Statically linking boost to a dynamic Ogre build doesn't work on Linux 64bit
		set(Boost_USE_STATIC_LIBS ${OGRE_STATIC})
	endif ()
	if (APPLE AND OGRE_BUILD_PLATFORM_APPLE_IOS)
	    set(Boost_USE_MULTITHREADED OFF)
	endif()
	set(Boost_ADDITIONAL_VERSIONS "1.54" "1.54.0" "1.53" "1.53.0" "1.52" "1.52.0" "1.51" "1.51.0" "1.50" "1.50.0" "1.49" "1.49.0" "1.48" "1.48.0" "1.47" "1.47.0" "1.46" "1.46.0" "1.45" "1.45.0" "1.44" "1.44.0" "1.42" "1.42.0" "1.41.0" "1.41" "1.40.0" "1.40")
	# Components that need linking (NB does not include header-only components like bind)
	set(OGRE_BOOST_COMPONENTS thread date_time)
	find_package(Boost COMPONENTS ${OGRE_BOOST_COMPONENTS} QUIET)
	if (NOT Boost_FOUND)
		# Try again with the other type of libs
		if(Boost_USE_STATIC_LIBS)
			set(Boost_USE_STATIC_LIBS OFF)
		else()
			set(Boost_USE_STATIC_LIBS ON)
		endif()
		find_package(Boost COMPONENTS ${OGRE_BOOST_COMPONENTS} QUIET)
	endif()
	
	if(Boost_FOUND AND Boost_VERSION GREATER 104900)
	        set(OGRE_BOOST_COMPONENTS thread date_time system chrono)
	        find_package(Boost COMPONENTS ${OGRE_BOOST_COMPONENTS} QUIET)
	endif()
	
	# Optional Boost libs (Boost_${COMPONENT}_FOUND
	macro_log_feature(Boost_FOUND "boost" "Boost (general)" "http://boost.org" FALSE "" "")
	macro_log_feature(Boost_THREAD_FOUND "boost-thread" "Used for threading support" "http://boost.org" FALSE "" "")
	macro_log_feature(Boost_DATE_TIME_FOUND "boost-date_time" "Used for threading support" "http://boost.org" FALSE "" "")
	if(Boost_VERSION GREATER 104900)
	    macro_log_feature(Boost_SYSTEM_FOUND "boost-system" "Used for threading support" "http://boost.org" FALSE "" "")
	    macro_log_feature(Boost_CHRONO_FOUND "boost-chrono" "Used for threading support" "http://boost.org" FALSE "" "")
	endif()
endif()


# Find DirectX
if (WIN32)
    # Find OpenGLES2
	find_package(OpenGLES2)
	macro_log_feature(OPENGLES2_FOUND "OpenGLES2" "Support for the OpenGLES2 render system" "" FALSE "" "")
    
	find_package(DirectX)
	macro_log_feature(DirectX9_FOUND "DirectX" "Support for the DirectX9 render system" "http://msdn.microsoft.com/en-us/directx/" FALSE "" "")

	find_package(DirectXInput)
	macro_log_feature(Direct_FOUND "DirectInput/XInput" "Support for the DirectInput/XInput" "http://msdn.microsoft.com/en-us/directx/" FALSE "" "")

	find_package(DirectX11)
	macro_log_feature(DirectX11_FOUND "DirectX11" "Support for the DirectX11 render system" "http://msdn.microsoft.com/en-us/directx/" FALSE "" "")
endif()


# Find Cg (Disabled Not used right now!)
# find_package(Cg)
# macro_log_feature(Cg_FOUND "cg" "C for graphics shader language" "http://developer.nvidia.com/object/cg_toolkit.html" FALSE "" "")

if(NOT WIN32 AND NOT OGREKIT_BUILD_ANDROID)
	# Use static loader On win32 platforms 

	# Find OpenAL
	include(FindOpenAL)
	macro_log_feature(OPENAL_FOUND "OpenAL" "Support for the OpenAL sound system" "http://connect.creativelabs.com/openal/default.aspx" FALSE "" "")
endif()

if(NOT OGREKIT_USE_STATIC_FREEIMAGE)
	find_package(FreeImage)
	macro_log_feature(FreeImage_FOUND "FreeImage" "Support for the FreeImage library" "http://freeimage.sourceforge.net/" FALSE "" "")

	find_package(ZLIB)
	macro_log_feature(ZLIB_FOUND "ZLib" "Support for the ZLib library" "http://www.zlib.net/" FALSE "" "")

endif()

#######################################################################
# Apple-specific
#######################################################################
if (APPLE AND NOT OGREKIT_BUILD_IPHONE)
#	find_package(Carbon)
#	macro_log_feature(Carbon_FOUND "Carbon" "Carbon" "http://developer.apple.com/mac" TRUE "" "")

#	find_package(Cocoa)
    FIND_LIBRARY(COCOA_LIBRARY Cocoa)
    FIND_LIBRARY(CARBON_LIBRARY Carbon)
    FIND_LIBRARY(IOKIT_LIBRARY IOKIT)
#	macro_log_feature(Cocoa_FOUND "Cocoa" "Cocoa" "http://developer.apple.com/mac" TRUE "" "")

#	find_package(IOKit)
#	macro_log_feature(IOKit_FOUND "IOKit" "IOKit HID framework needed by the samples" "http://developer.apple.com/mac" FALSE "" "")
endif()
	
# Display results, terminate if anything required is missing
MACRO_DISPLAY_FEATURE_LOG()

if (NOT OGREKIT_BUILD_MOBILE)

	# Add library and include paths from the dependencies
	include_directories(
		${OPENGL_INCLUDE_DIRS}
		${Cg_INCLUDE_DIRS}
		${X11_INCLUDE_DIR}
		${DirectX9_INCLUDE_DIRS}
		${DirectX11_INCLUDE_DIRS}
		${Carbon_INCLUDE_DIRS}
		${Cocoa_INCLUDE_DIRS}
	)
	
	link_directories(
		${OPENGL_LIBRARY_DIRS}
		${Cg_LIBRARY_DIRS}
		${X11_LIBRARY_DIRS}
		${DirectX9_LIBRARY_DIRS}
		${DirectX11_LIBRARY_DIRS}
	)

endif()
