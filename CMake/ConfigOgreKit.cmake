#reset the global vars
set(ADDONS_PATH "" CACHE INTERNAL "")
set(ADDONS "" CACHE INTERNAL "")

macro (add_addon addon sourcedir)
       list (APPEND ADDONS ${addon})
       list (APPEND ADDONS_PATH ${sourcedir})
       message(STATUS "ADDED new addon:${addon} => ${ADDONS}")

       #set(ADDONS ${ADDONS} PARENT_SCOPE)
       set(ADDONS ${ADDONS} CACHE INTERNAL "")
       set(ADDONS_PATH ${ADDONS_PATH} CACHE INTERNAL "")

endmacro()

macro (add_thrift THRIFTFILE_DIR THRIFTFILE_NAME THRIFT_OUTDIR SERVICE_PREFIX)

if (UNIX)
	set (EXE thrift)
elseif (MINGW)
	set (EXE thrift)
elseif (MSVC)
	set (EXE ${CMAKE_SOURCE_DIR}/Tools/executables/win32/thrift-0.9.0.exe)
endif()

  ADD_CUSTOM_COMMAND(
    OUTPUT  ${THRIFT_OUTDIR}/gen-cpp/${SERVICE_PREFIX}.cpp
            ${THRIFT_OUTDIR}/gen-cpp/${SERVICE_PREFIX}.h
            ${THRIFT_OUTDIR}/gen-cpp/${THRIFTFILE_NAME}_constants.cpp
            ${THRIFT_OUTDIR}/gen-cpp/${THRIFTFILE_NAME}_constants.h
            ${THRIFT_OUTDIR}/gen-cpp/${THRIFTFILE_NAME}_types.cpp
            ${THRIFT_OUTDIR}/gen-cpp/${THRIFTFILE_NAME}_types.h
            
    COMMAND ${EXE}
    ARGS -o ${THRIFT_OUTDIR} --gen cpp ${THRIFTFILE_DIR}/${THRIFTFILE_NAME}.thrift
    VERBATIM )


endmacro()

macro (write_addon_file filepath)
	message(STATUS "Generate addons file for addons:${ADDONS}")

	file(WRITE ${filepath} "#include \"gkEngine.h\"\n")
	foreach(addon ${ADDONS})
		message (STATUS "EACH ${addon}")
		file(APPEND ${filepath} "#include \"${addon}.h\"\n")
	endforeach()

	file(APPEND ${filepath} "class gkAddonAdder {\n")

		foreach(addon ${ADDONS})
			string(TOLOWER ${addon} lowername)		
			file(APPEND ${filepath} "static ${addon} ${lowername};\n")
		endforeach()

	file(APPEND ${filepath} "public: static void addAddons(){\n")
		foreach(addon ${ADDONS})
			string(TOLOWER ${addon} lowername)

			file(APPEND ${filepath} "if (gkEngine::getSingletonPtr()) gkEngine::getSingleton().addListener(&${lowername});\n")
		endforeach()
	file(APPEND ${filepath} "}\n\n")


	file(APPEND ${filepath} "static void setupAddons(){\n")
		foreach(addon ${ADDONS})
			string(TOLOWER ${addon} lowername)

			file(APPEND ${filepath} "	 if (gkEngine::getSingletonPtr()) ${lowername}.setup();\n")
		endforeach()
	file(APPEND ${filepath} "}\n\n")


	file(APPEND ${filepath} "static void removeAddons(){\n")
		foreach(addon ${ADDONS})
			string(TOLOWER ${addon} lowername)

			file(APPEND ${filepath} "if (gkEngine::getSingletonPtr()) gkEngine::getSingleton().removeListener(&${lowername});\n")
		endforeach()
	file(APPEND ${filepath} "}\n\n")


	file(APPEND ${filepath} "};\n")

foreach(addon ${ADDONS})
	string(TOLOWER ${addon} lowername)		
	file(APPEND ${filepath} "${addon} gkAddonAdder::${lowername};\n")
endforeach()

endmacro()

macro (configure_ogrekit ROOT OGREPATH)
	#message(STATUS ${OGREPATH})
	set(GNUSTEP_SYSTEM_ROOT $ENV{GNUSTEP_SYSTEM_ROOT})
	
set(ADDONS "" CACHE STRING 
			"set that get compiled into the runtime"
		)

	if(APPLE OR GNUSTEP_SYSTEM_ROOT)
		if (WIN32 AND NOT CMAKE_COMPILER_IS_GNUCXX)
			set(OGREKIT_USE_COCOA FALSE CACHE BOOL "Forcing remove Use Cocoa" FORCE)
		else()
			option(OGREKIT_USE_COCOA	"Use Cocoa"	ON)
		endif()
	endif()
	
	if(OGREKIT_USE_COCOA)
		add_definitions(-DOGREKIT_USE_COCOA)
		if(GNUSTEP_SYSTEM_ROOT)
			include_directories(${GNUSTEP_SYSTEM_ROOT}/Library/Headers)
			link_directories(${GNUSTEP_SYSTEM_ROOT}/Library/Libraries)
			link_libraries("gnustep-base")
			link_libraries("objc")
		endif()
        message(STATUS "--==##THE COCOCA: ${COCOA_LIBRARY}")
	endif()

	if (CMAKE_BUILD_TYPE STREQUAL "")
	  # CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
	  # differentiation between debug and release builds.
	  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
	endif ()

	set(OGREKIT_INSTALL_PREFIX ${ROOT}/Bin)
	#set(OGREKIT_USE_FILETOOLS TRUE)
	
	option(OGREKIT_USE_LUA					"Use Lua script bindings" ON)
	set(OGREKIT_EXTERNAL_LUA_LIB            "" CACHE FILEPATH "External static lib that should be used for linking lua instead of the buildin version")
	mark_as_advanced(OGREKIT_EXTERNAL_LUA_LIB)
	set (OGREKIT_PENDENCIES_DIR         "${OGREKIT_SOURCE_DIR}/../gamekit-pendencies" CACHE PATH "Path to gamekit-pendencies directory")
	option(OGREKIT_USE_THEORA			"Compile OgreTheora-Plugin" OFF)	
	option(OGREKIT_COMPILE_SWIG				"Enable compile time SWIG generation."  ON)
	option(OGREKIT_COMPILE_OGRE_SCRIPTS		"Automatically convert Blender TX to Ogre (.material, .font, .overlay... etc)" ON)
	option(OGREKIT_COMPILE_WXWIDGETS		"Enable / Disable wxWidgets builds" OFF)
	option(OGREKIT_DEBUG_ASSERT				"Enable / Disable debug asserts." ON)
	option(OGREKIT_HEADER_GENERATOR			"Build Blender DNA to C++ generator."   OFF)
	option(OGREKIT_UPDATE_CPP_DOCS			"Update C++ API documentation(Requires doxygen)." OFF)
	option(OGREKIT_UPDATE_LUA_DOCS			"Update Lua API documentation(Requires doxygen)." OFF)
	option(OGREKIT_UPDATE_FBT_DOCS			"Update FBT API documentation(Requires doxygen)." OFF)
	option(OGREKIT_DISABLE_ZIP				"Disable external .zip resource loading" ON)
	option(OGREKIT_USE_STATIC_FREEIMAGE		"Compile and link statically FreeImage and all its plugins" ON)	
	option(OGREKIT_MINIMAL_FREEIMAGE_CODEC	"Compile minimal FreeImage Codec(PNG/JPEG/TGA)" ON)
	option(OGREKIT_ENABLE_UNITTESTS			"Enable / Disable UnitTests" OFF)
	#option(OGREKIT_USE_FILETOOLS			"Compile FBT file format utilities" ON)
	# CAUTION: As of the blender 2.63-update bparse do not work for the moment! So set FBT as default for now
	option(OGREKIT_USE_BPARSE				"Compile bParse file format utilities" OFF) #FBT alternative 
	option(OGREKIT_COMPILE_TINYXML			"Enable / Disable TinyXml builds" OFF)
	option(OGREKIT_COMPILE_LIBROCKET		"Enable / Disable libRocket builds" ON)
	option(OGREKIT_COMPILE_GUI3D			"Enable / Disable GUI3D/Gorilla builds" ON)
	option(OGREKIT_GENERATE_BUILTIN_RES		"Generate build-in resources" ON)
	option(OGREKIT_COMPILE_TCL				"Compile TemplateGenerator" OFF)
	option(OGREKIT_COMPILE_RECAST			"Enable / Disable Recast build" ON)
	option(OGREKIT_COMPILE_OPENSTEER		"Enable / Disable OpenSteer build" ON)
	option(OGREKIT_USE_PROCESSMANAGER       "Enable / Disable ProcessManager build" ON)
	option(OGREKIT_COMPILE_SOFTBODY			"Eanble / Disable Bullet Softbody build" OFF)
	option(OGREKIT_USE_NNODE				"Use Logic Node(It's Nodal Logic, not Blender LogicBrick)" ON)
	option(OGREKIT_USE_PARTICLE				"Use Paritcle" ON)
	option(OGREKIT_COMPILE_OGRE_COMPONENTS	"Eanble compile additional Ogre components (RTShader, Terrain, Paging, ... etc)" OFF)
	option(OGREKIT_USE_RTSHADER_SYSTEM		"Eanble shader system instead of fixed piped functions." ON)
	option(OGREKIT_USE_COMPOSITOR			"Enable post effect by compositor (Bloom, BlackAndWhite, HDR, ...)" OFF)
	option(OGREKIT_USE_COMPOSITOR_TEX		"Add Compositor texture resources (NightVision, HeatVision, ...)" OFF)
	option(OGRE_BUILD_PLUGIN_CG		"Compile CGProgramManager" OFF)

	option(OGREKIT_COMPILE_OPTS				"Enable / Disable Opts builds" OFF)	
    option(OGREKIT_NETWORK_ENET_SUPPORT                     "Enable / Disable Network-Support via enet" ON)
	option(OGREKIT_UNITY_BUILD				"Enable / Dsiable Unity builds for OgreKit" OFF)			
	option(OGRE_UNITY_BUILD					"Enable / Dsiable Unity builds for Ogre" ON)
	option(OGREKIT_COMPILE_LIBCURL          "Compile libcurl for supporting multiple network-protocols" ON)

    option(OGREKIT_COMPILE_SKYX             "Compile SkyX" off)
    option(OGREKIT_COMPILE_HYDRAX           "Compile HydraX" off)

    option(OGREKIT_COMPILE_PROTOCOLBUFFERS   "Compile protocolbuffers" OFF)
    option(OGREKIT_COMPILE_THRIFT   "Compile thrift library" OFF)
    option(OGREKIT_USE_BOOST "Use boost" ON)

	option(OGREKIT_OPENAL_SOUND  "OpenAL support"   ON)


    option(RELEASE_MODE "RELEASE_MODE: only game.dat " OFF)

	option(OGRE_CONFIG_ENABLE_GLES2_GLSL_OPTIMISER              "Enable glsl-optimiser" OFF)

    option(OGREKIT_BUILD_OGREPROCEDURAL     "Integrate OgreProcedural" ON)
    
	set(OGRE_UNITY_FILES_PER_UNIT "40" CACHE STRING "Number of files per compilation unit in Unity build.")
	
	if (APPLE)
		option(OGREKIT_BUILD_IPHONE	"Build GameKit on iOS SDK"	OFF)
		option(OGREKIT_BUILD_IPHONE_UNIV "Support arm6 architecture for old devcie" OFF)
	endif()
	
	if (EMSCRIPTEN) 
		option(OGREKIT_BUILD_EMSCRIPTEN "Build Emscripten" ON)
		set (OGREKIT_EMSCRIPTEN_VIRTUAL_DIR "" CACHE PATH "Emscripten preload-directory")
		option(OGREKIT_EMSCRIPTEN_OUTPUT_HTMLFILE OFF)	
		set(OGREKIT_COMPILE_LIBCURL FALSE CACHE BOOL "" FORCE)
		set(OGREKIT_USE_OPENSSL FALSE CACHE BOOL "" FORCE)
		set(OGREKIT_NETWORK_ENET_SUPPORT FALSE CACHE BOOL "" FORCE)
	endif()

if (ANDROID)
	option(OGREKIT_BUILD_ANDROID	"Build GameKit on Android SDK"	ON)
else()
	option(OGREKIT_BUILD_ANDROID	"Build GameKit on Android SDK"	OFF)
endif()

if (BLACKBERRY)
	option(OGREKIT_BUILD_BLACKBERRY	"Build GameKit on Blackberry SDK"	ON)
else()
	option(OGREKIT_BUILD_BLACKBERRY	"Build GameKit on Blackberry SDK"	OFF)
endif()

	if (OGREKIT_BUILD_ANDROID OR OGREKIT_BUILD_IPHONE OR OGREKIT_BUILD_BLACKBERRY)
	    set(OGRE_BUILD_PLUGIN_CG 0)
		set(OGREKIT_BUILD_MOBILE 1)
	    option(OGRE_CONFIG_ENABLE_GLES2_GLSL_OPTIMISER              "Enable glsl-optimiser" ON)
    else()
        set(OGRE_CONFIG_ENABLE_GLES2_GLSL_OPTIMISER              0)
	endif()
    

    
    if (OGREKIT_BUILD_GLES2RS)
        set(OGREKIT_USE_RTSHADER_SYSTEM TRUE CACHE BOOL "Forcing RTShaderSystem" FORCE)
    endif()
	
	if (OGREKIT_USE_RTSHADER_SYSTEM OR OGREKIT_USE_COMPOSITOR)
		set(OGREKIT_DISABLE_ZIP CACHE BOOL "Forcing ZZLib" FORCE)
	endif()
	
	if (OGREKIT_USE_RTSHADER_SYSTEM)
		set(OGRE_BUILD_COMPONENT_RTSHADERSYSTEM TRUE)
		set(RTSHADER_SYSTEM_BUILD_CORE_SHADERS 1)
		set(RTSHADER_SYSTEM_BUILD_EXT_SHADERS 1)				
		
		if (OGREKIT_BUILD_MOBILE)
			message(STATUS "mobile rtshader")
			set(OGRE_BUILD_RENDERSYSTEM_GLES CACHE BOOL "Forcing OpenGLES" FORCE)
			set(OGRE_BUILD_RENDERSYSTEM_GLES2 TRUE CACHE BOOL "Forcing OpenGLES2" FORCE)

			set(OGREKIT_BUILD_GLESRS  CACHE BOOL "Forcing remove GLES"   FORCE)
			set(OGREKIT_BUILD_GLES2RS TRUE CACHE BOOL "Forcing OpenGLES2" FORCE)
        endif()

	endif()	
	
	
	if (OGREKIT_COMPILE_OGRE_COMPONENTS)
		option(OGRE_BUILD_COMPONENT_PAGING "Build Ogre Paging Compoment" ON)
		option(OGRE_BUILD_COMPONENT_TERRAIN "Build Ogre Terrain Compoment" ON)
		#option(OGRE_BUILD_COMPONENT_RTSHADERSYSTEM "Build Ogre RTShaderSystem Compoment" OFF)
		option(OGRE_BUILD_COMPONENT_PROPERTY "Build Ogre Property Compoment(Required boost)" OFF)
	endif()
	
	set(OGREKIT_ZZIP_TARGET ZZipLib)
	set(OGREKIT_FREETYPE_TARGET freetype)
	#set(OGREKIT_RECAST_TARGET Recast Detour DetourCrowd DetourSceneCreator RecastDetourDebugUtils)
	set(OGREKIT_RECAST_TARGET Recast )
	#set(OGREKIT_DETOUR_TARGET Detour)
	set(OGREKIT_OPENSTEER_TARGET OpenSteer)
	set(OGREKIT_LUA_TARGET Lua)
	set(OGREKIT_OGGVORBIS_TARGET OggVorbis)
	set(OGREKIT_OIS_TARGET OIS)
	set(OGREKIT_TINYXML_TARGET TinyXml)
	
	set(OGREKIT_DEP_DIR ${ROOT}/Dependencies/Source)
	set(OGRE_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/Bin)
	set(OGRE_TEMPLATES_DIR ${ROOT}/CMake/Templates)
	set(OGRELITE_SOURCE_DIR ${OGREPATH})
	set(OGRE_EXTERNAL_LIBS Ogre1.9a/External)
	set(OGREKIT_DEP_DIR ${ROOT}/Dependencies/Source)
	set(OGREKIT_SAMPLES_DIR ${ROOT}/Samples)
	set(OGREKIT_ANDROID_DEP_DIR ${ROOT}/Dependencies/Android)

option(OGREKIT_USE_OPENSSL        "COMPILE OPENSSL" ON)	
#if(OGREKIT_USE_OPENSSL)
   option(OGREKIT_COMPILE_OPENSSL ON)
#endif()

if (OGREKIT_COMPILE_THRIFT)
	set (OGREKIT_USE_BOOST 1)
endif()

if (OGREKIT_USE_OPENSSL)
    #set this to a default value, which might be overwritten
    set (OPENSSL_INCLUDE_DIR ${OGREKIT_DEP_DIR}/openssl/lib/include)

	if (OGREKIT_COMPILE_OPENSSL)
    	message(STATUS "OPENSSL: Want to compile the dependency")
    	set (OPENSSL_FOUND 1)
        set (OPENSSL_INCLUDE_DIR ${OGREKIT_DEP_DIR}/openssl/lib/include)
    	set (OPENSSL_LIBRARIES ssl crypto)
    	link_libraries(${OPENSSL_LIBRARIES})
	else()
		if (MINGW) 
			set (OPENSSL_FOUND 1)
			set (OPENSSL_INCLUDE_DIR ${OGREKIT_DEP_DIR}/openssl/lib/include)
			set (OPENSSL_LIBRARIES ssl crypto ws2_32 )
			MESSAGE(STATUS "SYSTEM NAME ${CMAKE_SYSTEM_NAME}")
			if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
				link_directories(${OGREKIT_DEP_DIR}/../Win32/openssl/mingw/compiled_on_win)			
			else()
				link_directories(${OGREKIT_DEP_DIR}/../Win32/openssl/mingw)			
			endif()
			#link_directories(${OGREKIT_DEP_DIR}/../Win32/winsock)			
			link_libraries(${OPENSSL_LIBRARIES})
			link_libraries(pthread)
			message(STATUS "OPENSSL: Using Win32-Precompiled Mingw-Openssl")
		elseif (MSVC)
			set (OPENSSL_FOUND 1)
			set (OPENSSL_INCLUDE_DIR ${OGREKIT_DEP_DIR}/openssl/lib/include)
			set (OPENSSL_LIBRARIES ssleay32 libeay32)
			link_directories(${OGREKIT_DEP_DIR}/../Win32/openssl/vc/lib)			
			link_libraries(${OPENSSL_LIBRARIES})
			message(STATUS "OPENSSL: Using Win32-Precompiled MSVC-Openssl")
		elseif (OGREKIT_BUILD_ANDROID)		
			set (OPENSSL_FOUND 1)
			set (OPENSSL_INCLUDE_DIR ${OGREKIT_DEP_DIR}/../android/include)
			set (OPENSSL_LIBRARIES ssl crypto)
			link_directories(${OGREKIT_DEP_DIR}/../android/openssl)			
			link_libraries(${OPENSSL_LIBRARIES})
			message(STATUS "OPENSSL: Using ANDROID-Precompiled Openssl-Libs")
		elseif (OGREKIT_BUILD_BLACKBERRY)
            set (OPENSSL_FOUND 1)
            set (OPENSSL_LIBRARIES ssl crypto)
            link_libraries(${OPENSSL_LIBRARIES})
            message(STATUS "OPENSSL: Using BB-Precompiled Openssl-Libs")
        elseif (UNIX)
            set (OPENSSL_FOUND 1)
            set (OPENSSL_LIBRARIES ssl crypto)
            link_libraries(${OPENSSL_LIBRARIES})
		endif()
	endif()
	
	list(APPEND OGREKIT_DEP_INCLUDE ${OPENSSL_INCLUDE_DIR})
	
endif()

	include(OgreConfigTargets)
	include(DependenciesOgreKit)
	include(MacroLogFeature)

      
message(STATUS "BOOST AFTER INCLUDE:${BOOST_INCLUDE_DIR}")
	
    if (OGREKIT_COMPILE_SKYX)
        include_directories(${OGREKIT_SOURCE_DIR}/${GAMEKIT_GRAPHIC_BACKEND_DIR}/External/SkyX/include)
    endif()

    if (OGREKIT_USE_BOOST)
##TODO: Those boost-path does not exist!?    
    
        if (OGREKIT_BUILD_ANDROID)
            set(OGREKIT_BOOST_ANDROID "${OGREKIT_PENDENCIES_DIR}/android/boost/include/boost-1_53" CACHE STRING "Boost-Directory for android" FORCE)
            include_directories(${OGREKIT_BOOST_ANDROID})
	 	elseif (MINGW)
            set(OGREKIT_BOOST_MINGW "/opt/mxe/usr/i686-pc-mingw32/include/boost" FORCE)
            include_directories(${OGREKIT_BOOST_MINGW})
            link_libraries(boost_system-mt)	
        elseif (MSVC)
            set(OGREKIT_BOOST_MSVC "${OGREKIT_PENDENCIES_DIR}/android/boost/include/boost-1_53" FORCE)
            #set(OGREKIT_BOOST_MSVC "C:/_dev/_libs/ThriftWin32/boost" FORCE)
            include_directories(${OGREKIT_BOOST_MSVC})	
        elseif(OGREKIT_BUILD_BLACKBERRY)
            set(OGREKIT_BOOST_ANDROID "${OGREKIT_PENDENCIES_DIR}/android/boost/include/boost-1_53" CACHE STRING "Boost-Directory for android" FORCE)
            message(STATUS "Using boost anndroid for blackberry (${OGREKIT_BOOST_ANDROID})")
            include_directories(${OGREKIT_BOOST_ANDROID})
        elseif(EMSCRIPTEN)
            if (NOT OGREKIT_BOOST_EMSCRIPTEN)
                set(OGREKIT_BOOST_EMSCRIPTEN "${OGREKIT_PENDENCIES_DIR}/android/boost/include/boost-1_53" CACHE PATH "Boost-Directory for emscripten" FORCE)
            endif()
            message(STATUS "Using boost for emscripten (${OGREKIT_BOOST_EMSCRIPTEN})")
            include_directories(${OGREKIT_BOOST_EMSCRIPTEN})
        endif()
    endif()

	if (NOT OGREKIT_USE_STATIC_FREEIMAGE)
		set (OGREKIT_FREEIMAGE_INCLUDE )
		if(ZLIB_FOUND)
			set(OGREKIT_ZLIB_TARGET	${ZLIB_LIBRARY})
			set(OGREKIT_FREEIMAGE_INCLUDE	${ZLIB_INCLUDE_DIR})
		else()
			message("Zlib not found.")
			message("Package is mandatory, please install it or enable static FreeImage compilation.")
		endif()
		
		
		if(FreeImage_FOUND)
			set(OGREKIT_FREEIMAGE_TARGET	${FreeImage_LIBRARY})	
			list(APPEND OGREKIT_FREEIMAGE_INCLUDE	${FreeImage_INCLUDE_DIR})
		else()
			message("FreeImage not found")
			message("Package is mandatory, please install it or enable static FreeImage compilation.")
		endif()
		
	else()
	
		set(OGREKIT_ZLIB_TARGET	ZLib)
		set(OGREKIT_FREEIMAGE_TARGET FreeImage)
		set(OGREKIT_ZLIB_INCLUDE ${OGREKIT_DEP_DIR}/FreeImage/ZLib)
		set(OGREKIT_FREEIMAGE_INCLUDE ${OGREKIT_DEP_DIR}/FreeImage)        
		
	endif()


	if (APPLE)
		set(CMAKE_XCODE_EFFECTIVE_PLATFORMS "-iphoneos;-iphonesimulator")	
		if (OGRE_BUILD_PLATFORM_IPHONE)
			option(OGREKIT_USE_COCOA "Use Cocoa" ON)
			set(OGREKIT_PLATFORM ${OGREPATH}/OgreMain/include/IPhone )
		else()
			set(OGREKIT_PLATFORM ${OGREPATH}/OgreMain/include/OSX )
		endif()
		
	else()
	
		if (UNIX)
			set(OGREKIT_PLATFORM ${OGREPATH}/OgreMain/include/GLX )
		elseif (WIN32)
			set(OGREKIT_PLATFORM ${OGREPATH}/OgreMain/include/WIN32 )
		endif()
		
	endif()
	
	option(SAMPLES_RUNTIME        "Build Samples/Runtime"       ON)
	option(SAMPLES_LOGICDEMO      "Build Samples/LogicDemo"     OFF)
	option(SAMPLES_VEHICLEDEMO    "Build Samples/VehicleDemo"   OFF)
	option(SAMPLES_CPPDEMO        "Build Samples/CppDemo"       OFF)
	option(SAMPLES_CPPDEMO_COMPILE_BLEND	"Convert Blend-Files to .inl-files and compile them in the executable" OFF)
	option(SAMPLES_NODE_EDITOR    "Build Samples/NodeEditor"    OFF)
	option(SAMPLES_EMBEDDEMO      "Build Samples/EmbedDemo"     OFF)
	option(SAMPLES_INSPECTOR      "Build Samples/FileInspector" OFF)
	option(SAMPLES_GUIDEMO        "Build Samples/GuiDemo"       OFF)
	option(SAMPLES_LUA_EDITOR     "Build Samples/LuaEditor"     OFF)
	option(SAMPLES_LUARUNTIME     "Build Samples/LuaRuntime"    OFF)
    option(SAMPLES_ANDROIDTEST    "Build Samples/Android/Test"  OFF)


	if (NOT SAMPLES_CPPDEMO)
		set(SAMPLES_CPPDEMO_COMPILE_BLEND FALSE CACHE BOOL "IF CPPDEMO not activated, deactivate cppdemo-compile blend as well" FORCE)	
	endif()

	if (SAMPLES_CPPDEMO_COMPILE_BLEND)
		set(OGREKIT_DISABLE_ZIP FALSE CACHE BOOL "Force GZ to enabled" FORCE)
		set(OGREKIT_GENERATE_BUILTIN_RES TRUE CACHE BOOL "Set GENERATE_BUILTIN_RES to true to enable the add_template-command" FORCE)
	endif()	

	
	if (SAMPLES_LOGICDEMO OR SAMPLES_VEHICLEDEMO)		
		set(OGREKIT_USE_NNODE TRUE CACHE BOOL "Forcing Logic Node" FORCE)		
	endif()
	
	if (SAMPLES_LOGICDEMO)
		set(OGREKIT_COMPILE_RECAST TRUE CACHE BOOL "Forcing Recast" FORCE)
		set(OGREKIT_COMPILE_OPENSTEER TRUE CACHE BOOL "Forcing OpenSteer" FORCE)
		set(OGREKIT_USE_PARTICLE TRUE CACHE BOOL "Forcing use Particle" FORCE)
	endif()
	
	if (SAMPLES_LUARUNTIME OR SAMPLES_LUA_EDITOR)
		set(OGREKIT_USE_LUA TRUE CACHE BOOL "Forcing Lua" FORCE)
	endif()
	
	if (SAMPLES_LUARUNTIME)
	endif()
	
	if (NOT OGREKIT_USE_LUA)
		set(OGREKIT_COMPILE_SWIG FALSE CACHE BOOL "Disabling Swig" FORCE)
	endif()

	if (SAMPLES_NODE_EDITOR OR SAMPLES_EMBEDDEMO OR SAMPLES_LUA_EDITOR OR SAMPLES_INSPECTOR)
		set(OGREKIT_COMPILE_WXWIDGETS TRUE CACHE BOOL "Forcing wxWidgets" FORCE)	
	endif()
	
	if (SAMPLES_LUA_EDITOR)
		set(OGREKIT_COMPILE_TINYXML TRUE CACHE BOOL "Forcing TinyXml" FORCE)
		set(SAMPLES_LUARUNTIME TRUE CACHE BOOL "Forcing LuaRuntime" FORCE)
	endif()

	if (SAMPLES_INSPECTOR)	
		set(OGREKIT_USE_FILETOOLS   TRUE CACHE BOOL "Forcing File Utils" FORCE)
	endif()

	if (WIN32 AND (SAMPLES_EMBEDDEMO OR SAMPLES_LUA_EDITOR))
		set(OGREKIT_OIS_WIN32_NATIVE FALSE CACHE BOOL "Forcing OISWin32Native"  FORCE)
	endif()
	
	if (SAMPLES_GUIDEMO)
		set(OGREKIT_COMPILE_LIBROCKET TRUE CACHE BOOL "Forcing LibRocket"  FORCE)
	endif()
	
	if (OGREKIT_COMPILE_SWIG OR OGREKIT_GENERATE_BUILTIN_RES)
		set(OGREKIT_COMPILE_TCL TRUE CACHE BOOL "Forcing TCL"  FORCE)
	endif()
	
	if (OGREKIT_BUILD_MOBILE)
	   set(OGREKIT_BUILD_GLRS    CACHE BOOL "Forcing remove GL"   FORCE)

		if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND OGREKIT_BUILD_MOBILE)
			add_definitions(-g -ggdb -O1)
			message(STATUS "Force DEBUG-Flag")
		endif()
	endif()

	# force -g for debug-mode		

    if (WIN32) 
		set(OGREKIT_BUILD_GLRS    TRUE CACHE BOOL "Forcing GLRS"   FORCE)
		set(OGREKIT_BUILD_GLESRS  FALSE CACHE BOOL "Forcing remove GLESRS"   FORCE)
		set(OGREKIT_BUILD_GLES2RS  FALSE CACHE BOOL "Forcing remove GLESRS"   FORCE)
	elseif (OGREKIT_BUILD_BLACKBERRY)
		set(OGREKIT_MINIMAL_FREEIMAGE_CODEC 1)

        set(BLACKBERRY_INCLUDE_DIR $ENV{QNX_TARGET}/usr/include)
        set(BLACKBERRY_LIB_DIR $ENV{QNX_TARGET}/${BB_ARCH}/usr/lib)
    
        include_directories(${BLACKBERRY_INCLUDE_DIR})
        link_directories(${BLACKBERRY_LIB_DIR})

		set(OGRE_BUILD_PLATFORM_BLACKBERRY TRUE)
		# check:
		  SET(TargetPlatform "BlackBerry")
		  SET(OGRE_BUILD_RENDERSYSTEM_GLES FALSE)
		  SET(OGRE_PLATFORM OGRE_PLATFORM_BLACKBERRY)
		  add_definitions(-DBLACKBERRY)
		 
		###
		
		set(OGREKIT_BUILD_GLRS    FALSE CACHE BOOL "Forcing GLRS"   FORCE)
		set(OGREKIT_BUILD_GLESRS  FALSE CACHE BOOL "Forcing remove GLESRS"   FORCE)
        	set(OGREKIT_BUILD_GLES2RS TRUE  CACHE BOOL "Forcing remove GLES2RS"   FORCE)
        
		set(OGREKIT_USE_RTSHADER_SYSTEM TRUE CACHE BOOL "Forcing RTShaderSystem for Android" FORCE)
		set(OGRE_CONFIG_ENABLE_VIEWPORT_ORIENTATIONMODE FALSE CACHE BOOL "Forcing viewport orientation support for Android" FORCE)
		if (NOT OGRE_CONFIG_ENABLE_VIEWPORT_ORIENTATIONMODE)
			set(OGRE_SET_DISABLE_VIEWPORT_ORIENTATIONMODE 1)
		endif()

		#message(${OGREKIT_BUILD_GLRS} "---" ${OGREKIT_BUILD_GLESRS} " --- " ${OPENGLES2_gl_LIBRARY})
	elseif (OGREKIT_BUILD_ANDROID)
	
		set(OGRE_BUILD_PLATFORM_ANDROID TRUE)

		#set(OGREKIT_OPENAL_SOUND   CACHE BOOL "Forcing remove OpenAL"   FORCE)
		
		set(OGREKIT_BUILD_GLRS    FALSE CACHE BOOL "Forcing GLRS"   FORCE)
		set(OGREKIT_BUILD_GLESRS  FALSE CACHE BOOL "Forcing remove GLESRS"   FORCE)
       		 set(OGREKIT_BUILD_GLES2RS TRUE  CACHE BOOL "Forcing remove GLES2RS"   FORCE)
        
		set(OGREKIT_USE_RTSHADER_SYSTEM TRUE CACHE BOOL "Forcing RTShaderSystem for Android" FORCE)
		set(OGRE_CONFIG_ENABLE_VIEWPORT_ORIENTATIONMODE FALSE CACHE BOOL "Forcing viewport orientation support for Android" FORCE)
		if (NOT OGRE_CONFIG_ENABLE_VIEWPORT_ORIENTATIONMODE)
			set(OGRE_SET_DISABLE_VIEWPORT_ORIENTATIONMODE 1)
		endif()


		#message(${OGREKIT_BUILD_GLRS} "---" ${OGREKIT_BUILD_GLESRS} " --- " ${OPENGLES2_gl_LIBRARY})
		
	elseif (OGREKIT_BUILD_IPHONE)
	
		set(OGRE_BUILD_PLATFORM_IPHONE TRUE) #TODO: replace to OGRE_BUILD_APPLE_IOS 
		set(OGRE_BUILD_PLATFORM_APPLE_IOS TRUE)
		
		#copy from ogre3d build
		# Set up iPhone overrides.
		include_directories("${OGREPATH}/OgreMain/include/iPhone")
	
		# Set build variables
		set(CMAKE_OSX_SYSROOT iphoneos)
		set(CMAKE_OSX_DEPLOYMENT_TARGET "")
		set(CMAKE_EXE_LINKER_FLAGS "-framework Foundation -framework CoreGraphics -framework QuartzCore -framework UIKit")
		set(XCODE_ATTRIBUTE_SDKROOT iphoneos)
		set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.yourcompany.\${PRODUCT_NAME:rfc1034identifier}")
		set(OGRE_CONFIG_ENABLE_VIEWPORT_ORIENTATIONMODE TRUE CACHE BOOL "Forcing viewport orientation support for iPhone" FORCE)
	
		# CMake 2.8.1 added the ability to specify per-target architectures.
		# As a side effect, it creates corrupt Xcode projects if you try do it for the whole project.
		if(VERSION STRLESS "2.8.1")
			set(CMAKE_OSX_ARCHITECTURES $(ARCHS_STANDARD_32_BIT))
		else()
			if (OGREKIT_BUILD_IPHONE_UNIV)
				set(CMAKE_OSX_ARCHITECTURES "armv6;armv7;i386")
			else()
				set(CMAKE_OSX_ARCHITECTURES "armv7;i386")
			endif()
		endif()
	
		add_definitions(-fno-regmove)
		remove_definitions(-msse)
	
		if (NOT OGRE_CONFIG_ENABLE_VIEWPORT_ORIENTATIONMODE)
			set(OGRE_SET_DISABLE_VIEWPORT_ORIENTATIONMODE 1)
		endif()
	
		if (OGREKIT_BUILD_GLES2RS)
			set(OGREKIT_BUILD_GLESRS FALSE CACHE BOOL "Forcing remove GLESRS"   FORCE)
		else()
			set(OGREKIT_BUILD_GLESRS TRUE CACHE BOOL "Forcing GLESRS"   FORCE)
		endif()
		

	elseif (APPLE)
	
		# Set 10.4 as the base SDK by default
		set(XCODE_ATTRIBUTE_SDKROOT macosx10.4)
	
		if (NOT CMAKE_OSX_ARCHITECTURES)
			set(CMAKE_OSX_ARCHITECTURES "i386")
		endif()
	  
		# 10.6 sets x86_64 as the default architecture.
		# Because Carbon isn't supported on 64-bit and we still need it, force the architectures to ppc and i386
		if(CMAKE_OSX_ARCHITECTURES MATCHES "x86_64" OR CMAKE_OSX_ARCHITECTURES MATCHES "ppc64")
			string(REPLACE "x86_64" "" CMAKE_OSX_ARCHITECTURES ${CMAKE_OSX_ARCHITECTURES})
			string(REPLACE "ppc64" "" CMAKE_OSX_ARCHITECTURES ${CMAKE_OSX_ARCHITECTURES})
		endif()
	
		# Make sure that the OpenGL render system is selected for non-iPhone Apple builds
		set(OGREKIT_BUILD_GLRS    TRUE  CACHE BOOL "Forcing GLRS"   FORCE)
		set(OGREKIT_BUILD_GLESRS  FALSE CACHE BOOL "Forcing remove GLESRS"   FORCE)
        set(OGREKIT_BUILD_GLES2RS FALSE CACHE BOOL "Forcing remove GLES2RS"   FORCE)
	elseif (EMSCRIPTEN)
	       include_directories(${BLACKBERRY_INCLUDE_DIR})
	endif ()


	if (OGREKIT_COMPILE_SWIG)
		include(RunSwig)
	endif()


	if (OGREKIT_COMPILE_TCL)	
		include(TemplateCompiler)
	endif()
	
	set(OGREKIT_FREETYPE_INCLUDE ${OGREKIT_DEP_DIR}/FreeType/include)
	set(OGREKIT_ZZIP_INCLUDE ${OGREKIT_DEP_DIR}/ZZipLib)
	set(OGREKIT_OIS_INCLUDE ${OGREKIT_DEP_DIR}/OIS/include)
	set(OGREKIT_OGRE_INCLUDE ${OGREPATH}/OgreMain/include ${OGREPATH}/PlugIns/CgProgramManager/include ${OGREPATH}/Components/Overlay/include ${OGREPATH}/Components/HLMS/include ${OGREKIT_BINARY_DIR}/include ${OGREKIT_PLATFORM})	
	set(OGREKIT_LUA_INCLUDE ${OGREKIT_DEP_DIR}/Lua/lua)
	set(OGREKIT_OGGVORBIS_INCLUDE ${OGREKIT_DEP_DIR}/Codecs/include)
	
	set(OGREKIT_RECAST_DIR ${OGREKIT_DEP_DIR}/RecastDetour/recastnavigation)
#	set(OGREKIT_RECAST_DIR ${OGREKIT_DEP_DIR}/masa_recast)
	set(OGREKIT_RECAST_INCLUDE ${GAMEKIT_ADDITIONAL_DEP_FOLDER}/recastnavigation/Recast/Include 
							   ${GAMEKIT_ADDITIONAL_DEP_FOLDER}/recastnavigation/DebugUtils/Include 
							   ${GAMEKIT_ADDITIONAL_DEP_FOLDER}/recastnavigation/DetourTileCache/Include 
							   ${GAMEKIT_ADDITIONAL_DEP_FOLDER}/recastnavigation/Detour/Include 
							   ${GAMEKIT_ADDITIONAL_DEP_FOLDER}/recastnavigation/RecastDemo/Include
							   ${GAMEKIT_ADDITIONAL_DEP_FOLDER}/recastnavigation/DetourCrowd/Include
							   #${OGREKIT_RECAST_DIR}/DetourSceneCreator/Include
							   )
#	set(OGREKIT_DETOUR_INCLUDE ${OGREKIT_DEP_DIR}/RecastDetour/recastnavigation/Detour/Include)

	set(OGREKIT_CPPTWEENER_INCLUDE ${OGREKIT_DEP_DIR}/CppTweener/include)
	set(OGREKIT_OPENSTEER_INCLUDE ${OGREKIT_DEP_DIR}/OpenSteer/include)
	set(OGREKIT_TINYXML_INCLUDE ${OGREKIT_DEP_DIR}/TinyXml)
	set(OGREKIT_OPTS_INCLUDE ${OGREKIT_DEP_DIR}/opts)
	set(OGREKIT_GUI3D_INCLUDE  ${GAMEKIT_ADDITIONAL_DEP_FOLDER}/gui3d/build/source)
    set(OGREKIT_LIBCURL_INCLUDE ${OGREKIT_DEP_DIR}/libcurl/include)
    set(OGREKIT_RAPIDJSON_INCLUDE ${OGREKIT_DEP_DIR}/rapidJSON/include)
    set(OGREKIT_PROTOBUF_INCLUDE ${OGREKIT_DEP_DIR}/ProtocolBuffers/protobuf/src)
    set(OGREKIT_THRIFT_INCLUDE ${OGREKIT_DEP_DIR}/thrift/src)
    set(OGREKIT_OPENSSL_INCLUDE ${OGREKIT_DEP_DIR}/openssl/lib/include)

	set(OGREKIT_DEP_INCLUDE
#		${OGREKIT_FREEIMAGE_INCLUDE} Conflicts with OpenSteer includes and needed by Ogre, not OgreKit
		${OGREKIT_FREETYPE_INCLUDE}
		${OGREKIT_ZLIB_INCLUDE}
		${OGREKIT_OIS_INCLUDE}				
		${GAMEKIT_UTILS_PATH}
		${GAMEKIT_ANIMKIT_PATH}	
		${OGREKIT_CPPTWEENER_INCLUDE}
		${OGREKIT_RAPIDJSON_INCLUDE}
		
		${OGREKIT_GUI3D_INCLUDE}	
	)
	
	if (OGREKIT_USE_BPARSE)
	
		list(APPEND OGREKIT_DEP_INCLUDE
			${GAMEKIT_SERIALIZE_BULLET}
			${GAMEKIT_SERIALIZE_BLENDER}
		)
	else()
		list(APPEND OGREKIT_DEP_INCLUDE
			${OGREKIT_SOURCE_DIR}/Tools/FileTools/File
			${OGREKIT_SOURCE_DIR}/Tools/FileTools/FileFormats/Blend
		)
	endif()



	
	set(OGREKIT_LIBROCKET_INCLUDE ${OGREKIT_DEP_DIR}/libRocket/Include ${OGREKIT_DEP_DIR}/libRocket/Source)
	set(OGREKIT_LIBROCKET_LIBS RocketCore RocketControls RocketDebugger)
	
	if (OGREKIT_COMPILE_LIBROCKET)
		list(APPEND OGREKIT_DEP_INCLUDE
			${OGREKIT_LIBROCKET_INCLUDE}
		)
	endif()

	if (OGREKIT_COMPILE_GUI3D)
		list(APPEND OGREKIT_DEP_INCLUDE
			${OGREKIT_GUI3D_INCLUDE}
		)
	endif()


	message(STATUS "DEP_INCLUDES: ${OGREKIT_DEP_INCLUDE}")
	
	if (WIN32 AND NOT OGREKIT_BUILD_ANDROID)
		# Use static library. No SDK needed at build time.
		# Must have OpenAL32.dll installed on the system 
		# In order to use OpenAL sound.
		set(OPENAL_FOUND TRUE)
	endif()

	if (OGREKIT_BUILD_BLACKBERRY)
		set(OPENAL_FOUND TRUE)
		set(OGREKIT_OPENAL_LIBRARY )
		# shared lib added via bar-description
    


		include_directories($ENV{QNX_TARGET}/usr/include/AL)
#		set(OGREKIT_OPENAL_LIBRARY OpenAL)
	elseif (OGREKIT_BUILD_EMSCRIPTEN)
  	    MESSAGE (STATUS "00000 Emscripten path:${EMSCRIPTEN_ROOT_PATH}")
		if (OGREKIT_OPENAL_SOUND)
			set(OGREKIT_OPENAL_INCLUDE ${EMSCRIPTEN_ROOT_PATH}/system/include/AL)
			set(OPENAL_FOUND true)
			set(OGREKIT_OPENAL_LIBRARY "")			
		endif()
	elseif (OGREKIT_BUILD_ANDROID)
		if (OGREKIT_OPENAL_SOUND)
		   	set(OGREKIT_OPENAL_INCLUDE ${OGREKIT_DEP_DIR}/OpenAL/)
			message(STATUS "ANDROID OPENAL HEADER: ${OGREKIT_OPENAL_INCLUDE}")
			set(OPENAL_FOUND true)
			if (NOT OPENAL_LIBRARY)
				set (OPENAL_LIBRARY "${OGREKIT_DEP_DIR}/../android/openal/armeabi-v7a/libopenal.so")
				message(STATUS "Set library to android-openal-lib to : ${OPENAL_LIBRARY}")
			endif()
			set(OGREKIT_OPENAL_LIBRARY ${OPENAL_LIBRARY})
		endif()
	elseif (OPENAL_FOUND) # total nonsense!???
		option(OGREKIT_OPENAL_SOUND "Enable building of the OpenAL subsystem" ON)
		
		if (WIN32)
			add_definitions(-DAL_STATIC_LIB -DALC_STATIC_LIB)
			set(OGREKIT_OPENAL_INCLUDE ${OGREKIT_DEP_DIR}/OpenAL/)
			set(OGREKIT_OPENAL_LIBRARY OpenAL)
		else()
			set(OGREKIT_OPENAL_INCLUDE ${OPENAL_INCLUDE_DIR})
			set(OGREKIT_OPENAL_LIBRARY ${OPENAL_LIBRARY})
		endif()
	else()
		option(OGREKIT_OPENAL_SOUND "Enable building of the OpenAL subsystem" OFF)
	endif()

	set(OGREKIT_MINGW_DIRECT3D TRUE)
	if (CMAKE_COMPILER_IS_GNUCXX)
		# Some Issues with unresolved symbols
		set(OGREKIT_MINGW_DIRECT3D FALSE)
	endif()


	if (WIN32)
	    SET(OGRE_BUILD_RENDERSYSTEM_GLES FALSE)
		if (NOT DirectX_FOUND OR NOT OGREKIT_MINGW_DIRECT3D)
			# Default use OIS without dinput 

			option(OGREKIT_OIS_WIN32_NATIVE "Enable building of the OIS Win32 backend" ON)
		else ()
			# Use standard OIS build.
			# CAUTION: For now there are some missing symbols, which work with native fine. So for now lets set it to native as default
			option(OGREKIT_OIS_WIN32_NATIVE "Enable building of the OIS Win32 backend" On)
		endif()
	endif()

	
	if (OPENGL_FOUND)
		option(OGREKIT_BUILD_GLRS "Enable the OpenGL render system" ON)
	endif()
	
	if (OPENGLES_FOUND)
		option(OGREKIT_BUILD_GLESRS "Enable the OpenGLES system" ON)
	endif()
	
    if (NOT OGREKIT_BUILD_MOBILE)
    	if (OPENGLES2_FOUND) 
			set(OGRE_BUILD_RENDERSYSTEM_GLES2 TRUE CACHE BOOL "Forcing OpenGLES2" FORCE)
			set(OGREKIT_BUILD_GLES2RS 		  TRUE CACHE BOOL "Forcing OpenGLES2" FORCE)	
		    message(STATUS "4 : ${OGREKIT_BUILD_GLES2RS} ${OGREKIT_BUILD_GLRS}")
		endif()
		
		if(OPENGLES_FOUND)
    		set(OGRE_BUILD_RENDERSYSTEM_GLES FALSE CACHE BOOL "Forcing OpenGLES" )
			set(OGREKIT_BUILD_GLESRS FALSE CACHE BOOL "Forcing remove GLES"   )
		endif()
    endif()
    
    message(STATUS "3 : ${OGREKIT_BUILD_GLES2RS} ${OGREKIT_BUILD_GLRS}")
    
#    if (OPENGLES2_FOUND)
#        if (MSVC)
#            option(OGREKIT_BUILD_GLES2RS "Enable the OpenGLES2 system" OFF)
#        else()
#            option(OGREKIT_BUILD_GLES2RS "Enable the OpenGLES2 system" ON)
#        endif()
#        
#        #mark_as_advanced(OGREKIT_BUILD_GLES2RS)
#	endif()
	
	message(STATUS "2 : ${OGREKIT_BUILD_GLES2RS} ${OGREKIT_BUILD_GLRS}")
	
	if (OGREKIT_BUILD_GLRS)
	   set(OGRE_BUILD_RENDERSYSTEM_GL TRUE  CACHE BOOL "Forcing OpenGL RenderSystem" FORCE)
	   
	   set(OGRE_BUILD_RENDERSYSTEM_GLES2 FALSE CACHE BOOL "Forcing OpenGLES2" FORCE)
	   set(OGREKIT_BUILD_GLES2RS         FALSE CACHE BOOL "Forcing OpenGLES2" FORCE)	
	else()
	   set(OGRE_BUILD_RENDERSYSTEM_GL FALSE CACHE BOOL "Forcing remove OpenGL RenderSystem" FORCE)
	endif()
	
	if (OGREKIT_BUILD_GLESRS)
	   set(OGRE_BUILD_RENDERSYSTEM_GLES TRUE  CACHE BOOL "Forcing OpenGLES RenderSystem" FORCE)
	else()
	   set(OGRE_BUILD_RENDERSYSTEM_GLES FALSE CACHE BOOL "Forcing remove OpenGLES RenderSystem" FORCE)
	endif()
	
	message(STATUS "1 : ${OGREKIT_BUILD_GLES2RS}")
	if (OGREKIT_BUILD_GLES2RS)	   
	   set(OGRE_BUILD_RENDERSYSTEM_GLES2 TRUE  CACHE BOOL "Forcing OpenGLES2 RenderSystem" FORCE)       
	else()
	   set(OGRE_BUILD_RENDERSYSTEM_GLES2 FALSE CACHE BOOL "Forcing remove OpenGLES2 RenderSystem" FORCE)
	endif()
	
	mark_as_advanced(
	   OGRE_BUILD_RENDERSYSTEM_GL 
	   OGRE_BUILD_RENDERSYSTEM_GLES 
	   OGRE_BUILD_RENDERSYSTEM_GLES2
	)

	if (OPENGL_FOUND AND OGREKIT_BUILD_GLRS)
		set(OGRE_BUILD_RENDERSYSTEM_GL  TRUE)
		set(OGREKIT_GLRS_LIBS           RenderSystem_GL)
		set(OGREKIT_GLRS_ROOT           ${OGREPATH}/RenderSystems/GL)
		set(OGREKIT_GLESRS_INCLUDE      ${OGREPATH}/RenderSystems/GLES/include)
		set(OGREKIT_GLRS_INCLUDE        ${OGREPATH}/RenderSystems/GL/include)
	endif()

	if (OPENGLES_FOUND AND OGREKIT_BUILD_GLESRS)		
		set(OGRE_BUILD_RENDERSYSTEM_GLES TRUE)
		set(OGREKIT_GLESRS_LIBS          RenderSystem_GLES)
		set(OGREKIT_GLESRS_ROOT          ${OGREPATH}/RenderSystems/GLES)
		set(OGREKIT_GLESRS_INCLUDE       ${OGREPATH}/RenderSystems/GLES/include)
		set(OGREKIT_GLRS_INCLUDE         ${OGREPATH}/RenderSystems/GL/include)
	endif()

	if (OPENGLES2_FOUND AND OGREKIT_BUILD_GLES2RS)	
		set(OGRE_BUILD_RENDERSYSTEM_GLES2  TRUE)
		set(OGREKIT_GLES2RS_LIBS           RenderSystem_GLES2)
		set(OGREKIT_GLES2RS_ROOT           ${OGREPATH}/RenderSystems/GLES2)
		set(OGREKIT_GLES2RS_INCLUDE        ${OGREPATH}/RenderSystems/GLES2/include)
		set(OGREKIT_RTSHADERSYSTEM_INCLUDE ${OGREPATH}/Components/RTShaderSystem/include)
		set(OGREKIT_GLRS_INCLUDE           ${OGREPATH}/RenderSystems/GL/include)
	endif()

	if (WIN32 AND OGREKIT_MINGW_DIRECT3D)

		if (DirectX9_FOUND)
			option(OGREKIT_BUILD_D3D9RS	 "Enable the Direct3D 9 render system" ON)
		endif()

		if (DirectX11_FOUND)
			option(OGREKIT_BUILD_D3D11RS "Enable the Direct3D 11 render system" OFF)
		endif()

		if (DirectX9_FOUND AND OGREKIT_BUILD_D3D9RS)
			set(OGRE_BUILD_RENDERSYSTEM_D3D9   TRUE)
			set(OGREKIT_D3D9_LIBS              RenderSystem_Direct3D9)
			set(OGREKIT_D3D9_ROOT              ${OGREPATH}/RenderSystems/Direct3D9)
			set(OGREKIT_DX9RS_INCLUDE          ${OGREPATH}/RenderSystems/Direct3D9/include)
		endif()


		if (DirectX11_FOUND AND OGREKIT_BUILD_D3D11RS)
			set(OGRE_BUILD_RENDERSYSTEM_D3D11  TRUE)
			set(OGREKIT_D3D11_LIBS             RenderSystem_Direct3D11)
			set(OGREKIT_D3D11_ROOT             ${OGREPATH}/RenderSystems/Direct3D11)
			set(OGREKIT_DX11RS_INCLUDE         ${OGREPATH}/RenderSystems/Direct3D11/include)
		endif()


	endif()
	

	set(OGREKIT_OGRE_LIBS 
		OgreMain       
	)    
	
	list(APPEND OGREKIT_OGRE_LIBS OgreOverlay)  

	list(APPEND OGREKIT_OGRE_LIBS OgreHLMS) 

    if (OGRE_BUILD_PLUGIN_CG)
        list(APPEND OGREKIT_OGRE_LIBS Plugin_CgProgramManager)  
    endif()

    if (OGREKIT_BUILD_OGREPROCEDURAL)
		list(APPEND OGREKIT_DEP_INCLUDE
			${OGRE_EXTERNAL_LIBS}/OgreProcedural/library/include
		)
	
		message(STATUS "__PROCEDUREAL INCLUDE:${OGRE_EXTERNAL_LIBS}/OgreProcedural/library/include")
		message(STATUS "CURRENT DEP_INCLUDE_${OGREKIT_DEP_INCLUDE}")
        list(APPEND OGREKIT_OGRE_LIBS OgreProcedural)  
    endif()

	list(APPEND OGREKIT_OGRE_LIBS 	${OGREKIT_FREEIMAGE_TARGET} 
		${OGREKIT_FREETYPE_TARGET}
		${OGREKIT_GLRS_LIBS}        
		${OGREKIT_D3D9_LIBS}
		${OGREKIT_D3D11_LIBS}		
		${GAMEKIT_UTILS_TARGET}
		${OGREKIT_OIS_TARGET}
		${OGREKIT_ZLIB_TARGET}
		${GAMEKIT_ANIMKIT_TARGET}
		cpptweener
	)

    if (OGREKIT_COMPILE_PROTOCOLBUFFERS) 
        list(APPEND OGREKIT_DEP_INCLUDE
            ${OGREKIT_PROTOBUF_INCLUDE}
        )
        link_directories(${CMAKE_BINARY_DIR}/Dependencies/Source/protobuf/lib)
        
        list(APPEND OGREKIT_OGRE_LIBS protobuf)
    endif()

    if (OGREKIT_LINK_OPENSSL) 
	#list(APPEND OGREKIT_OGRE_LIBS ssl crypto)
        if (MSVC)
			list(APPEND OGREKIT_DEP_INCLUDE
				${CMAKE_SOURCE_DIR}/Dependencies/Win32/openssl/vc/include
			)
		else()
			list(APPEND OGREKIT_DEP_INCLUDE
				${OGREKIT_OPENSSL_INCLUDE}
			)
		endif()
    endif()

    if (OGREKIT_COMPILE_THRIFT) 
        message(STATUS "COMPILE THRIFT ${OGREKIT_THRIFT_INCLUDE}")
        list(APPEND OGREKIT_DEP_INCLUDE
            ${OGREKIT_THRIFT_INCLUDE}
        )
        
        
        #link_directories(${CMAKE_BINARY_DIR}/Dependencies/Source/protobuf/lib)
        if (WIN32) 
        	link_directories(${CMAKE_SOURCE_DIR}/Dependencies/Win32/libthrift-0.9.0)
	endif()
        list(APPEND OGREKIT_OGRE_LIBS thrift)
    endif()
	
	if (OGREKIT_COMPILE_LIBCURL)

	   if (OGREKIT_BUILD_ANDROID) 
			# generated config-files
			include_directories(${OGREKIT_DEP_DIR}/../android/include)
			# precompiled lib
			link_directories(${OGREKIT_DEP_DIR}/../android/libcurl/arm)
			message(STATUS "android-libcurl-libpath: ${OGREKIT_DEP_DIR}/../android/libcurl/arm" )
	   elseif(OGREKIT_BUILD_BLACKBERRY)

            
            set (LIBCURL_INCLUDE_DIR $ENV{QNX_TARGET}/usr/include)

    
            link_libraries(curl)
			
            message(STATUS "blackberry-libcurl-include-path: $ENV{QNX_TARGET}/usr/include" )          

	   elseif(APPLE)
            link_libraries(curl)
            
            message(STATUS "blackberry-libcurl-include-path: $ENV{QNX_TARGET}/usr/include" )  	   
       elseif(MINGW)
        set (LIBCURL_INCLUDE_DIR ${ZLIB_INCLUDE_DIR})
		#include_directories(${OGREKIT_DEP_DIR}/libcurl/config/mingw)
		message(STATUS "...using MINGW CURL ${ZLIB_INCLUDE_DIR}")
		link_libraries(curl)
		set (OGREKIT_COMPILE_LIBCURL FALSE)
	   else()
		include_directories(${OGREKIT_DEP_DIR}/libcurl/config/desktop)
	   endif()
	   
	   if(WIN32)
		list(APPEND OGREKIT_OGRE_LIBS curl)
	   else()
		list(APPEND OGREKIT_OGRE_LIBS curl)
       endif()

			   
	   if (NOT OGREKIT_BUILD_MOBILE) # mobile use their own libcurl includes
		   list(APPEND OGREKIT_DEP_INCLUDE
		    # curl_build.h
            ${CMAKE_BINARY_DIR}/Depedencies/Source/libcurl/include
            # curl_config.h
            ${CMAKE_BINARY_DIR}/Depedencies/Source/libcurl/lib
            # the libary
		    ${OGREKIT_LIBCURL_INCLUDE}
		   )
           endif()
	endif()

	if (OGRE_CONFIG_ENABLE_GLES2_GLSL_OPTIMISER)
		list(APPEND OGREKIT_DEP_INCLUDE
			${OGREKIT_DEP_DIR}/glsl-optimiser/src/glsl
		)
		set(GLSL_Optimizer_LIBRARIES mesa glsl_optimizer glcpp-library)
		list(APPEND OGREKIT_OGRE_LIBS ${GLSL_Optimizer_LIBRARIES})

	endif()	

	if (OGREKIT_COMPILE_GUI3D)
		list(APPEND OGREKIT_OGRE_LIBS GUI3D)  
	endif()

	if (OGREKIT_USE_BPARSE)
		set(BPARSE_FILE_FORMAT 2 CACHE STRING 
			"Select the bparse-blendfile-format:
				 1 - <= 2.62
				 2 - >= 2.63
			"
		)
		list(APPEND OGREKIT_OGRE_LIBS
			${GAMEKIT_SERIALIZE_BLENDER_TARGET}
			${GAMEKIT_SERIALIZE_BULLET_TARGET}
		)
	else()
		list(APPEND OGREKIT_OGRE_LIBS
			${GAMEKIT_FBT_LIBS}
		)
	endif()

	if (OGREKIT_USE_RTSHADER_SYSTEM)
		list(APPEND OGREKIT_OGRE_LIBS		OgreRTShaderSystem)
		list(APPEND OGREKIT_OGRE_INCLUDE	${OGREPATH}/Components/RTShaderSystem/include)
	endif()

	if (OGREKIT_OPENAL_SOUND)
		list(APPEND OGREKIT_OGRE_LIBS	${OGREKIT_OPENAL_LIBRARY} ${OGREKIT_OGGVORBIS_TARGET})
		list(APPEND OGREKIT_DEP_INCLUDE ${OGREKIT_OPENAL_INCLUDE} ${OGREKIT_OGGVORBIS_INCLUDE})
	endif()
		
	if (OGREKIT_USE_LUA)
		list(APPEND OGREKIT_DEP_INCLUDE ${OGREKIT_LUA_INCLUDE})
		if ("${OGREKIT_EXTERNAL_LUA_LIB}" STREQUAL "")
			list(APPEND OGREKIT_OGRE_LIBS	${OGREKIT_LUA_TARGET})
		else()
			message(STATUS "LUA: Using external lib: ${OGREKIT_EXTERNAL_LUA_LIB}")
			link_libraries(${OGREKIT_EXTERNAL_LUA_LIB})
		endif()
	endif()

	if (NOT OGREKIT_DISABLE_ZIP)
		list(APPEND OGREKIT_OGRE_LIBS	${OGREKIT_ZZIP_TARGET})
		list(APPEND OGREKIT_DEP_INCLUDE ${OGREKIT_ZZIP_INCLUDE})
	endif()

	if (OGREKIT_USE_THEORA)
		list(APPEND OGREKIT_DEP_INCLUDE ${OGRE_EXTERNAL_LIBS}/TheoraPlugin/include ${OGRE_EXTERNAL_LIBS}/TheoraPlugin/theoraplayer/include)
	endif()

	if (OGREKIT_COMPILE_RECAST)
		MESSAGE(STATUS "##__## COMPILE RECAST ${OGREKIT_RECAST_INCLUDE}")
		list(APPEND OGREKIT_OGRE_LIBS	${OGREKIT_RECAST_TARGET} )
		list(APPEND OGREKIT_DEP_INCLUDE ${OGREKIT_RECAST_INCLUDE} ${OGREKIT_DETOUR_INCLUDE})
	endif()
	
	if (OGREKIT_COMPILE_OPENSTEER)
		list(APPEND OGREKIT_OGRE_LIBS	${OGREKIT_OPENSTEER_TARGET})
		list(APPEND OGREKIT_DEP_INCLUDE ${OGREKIT_OPENSTEER_INCLUDE})
	endif()
		
	configure_file(${ROOT}/CMake/Templates/OgreKitSettings.in ${CMAKE_BINARY_DIR}/Engine/gkSettings.h)
	include_directories(${ROOT}/Engine ${CMAKE_BINARY_DIR}/Engine)
		
	#Check Build Settings
	if (APPLE)
		if (OGREKIT_BUILD_IPHONE)
						
			if (OGREKIT_BUILD_GLRS)
				message(SEND_ERROR "Turn OFF OGREKIT_BUILD_GLRS Option for iOS")
			endif()
			if (NOT OGREKIT_BUILD_GLESRS AND NOT OGREKIT_BUILD_GLES2RS)
				message(SEND_ERROR "Turn ON OGREKIT_BUILD_GLESRS or OGREKIT_BUILD_GLES2RS Option for iOS")
			endif()
			
			if(VERSION STRLESS "2.8.2")
				message(ERROR "You should update the CMake 2.8.2 higher for iOS build.")
	  		endif()
	  
		else()
			
			if (NOT OGREKIT_BUILD_GLRS)
				message(SEND_ERROR "Turn ON OGREKIT_BUILD_GLRS Option for OS X")
			endif()
			if (OGREKIT_BUILD_GLESRS OR OGREKIT_BUILD_GLESRS)
				message(SEND_ERROR "Turn OFF OGREKIT_BUILD_GLESRS or OGREKIT_BUILD_GLES2RS Option for OS X")
			endif()

		endif()
	endif(APPLE)

endmacro(configure_ogrekit)


macro(configure_rendersystem)

	if (OGRE_BUILD_RENDERSYSTEM_GL)
		
		include_directories(
			${OGREKIT_GLRS_ROOT}/include
			${OGREKIT_GLRS_ROOT}/src/GLSL/include
			${OGREKIT_GLRS_ROOT}/src/atifs/include
		)
	
		link_libraries(
			${OGREKIT_GLRS_LIBS} 
			${OPENGL_gl_LIBRARY}
			${OPENGL_glu_LIBRARY}
		)
		
	endif()

	if (OGRE_BUILD_RENDERSYSTEM_GLES)
		
		include_directories(
			${OGREKIT_GLESRS_ROOT}/include
			${OGREKIT_GLESRS_ROOT}/include/EAGL
		)
	
		link_libraries(
			${OGREKIT_GLESRS_LIBS} 
		)
		
	endif()

	if (OGRE_BUILD_RENDERSYSTEM_GLES2)
		
		include_directories(
			${OGREKIT_GLES2RS_ROOT}/include
			${OGREKIT_RTSHADERSYSTEM_INCLUDE}
		)
	
		link_libraries(
			${OGREKIT_GLES2RS_LIBS} 
		)
        
        if (NOT OGREKIT_BUILD_MOBILE)
            include_directories(${OPENGLES2_INCLUDE_DIR})
            #link_directories(${OPENGLES2_LIBRARY_PATH})
            list(APPEND OGREKIT_OGRE_LIBS ${OPENGLES_gl_LIBRARY})
            #message(STATUS ${OGREKIT_OGRE_LIBS})
        endif()
		
		message(STATUS "--------" ${OGREKIT_GLES2RS_LIBS} )
		
	endif()
	
	if (OGREKIT_BUILD_D3D9RS)

		include_directories(
			${OGREKIT_D3D9_ROOT}/include
		)
		
		link_libraries(
			${DirectX9_D3D9_LIBRARY}
			${OGREKIT_D3D9_LIBS} 
		)
			
	endif()
	

	if (OGREKIT_BUILD_D3D11RS) # AND DirectX_D3D11_FOUND)
		
		include_directories(
			${OGREKIT_D3D11_ROOT}/include
		)
		
		link_libraries(
			${OGREKIT_D3D11_LIBS} 
			#${DirectX_D3D10_LIBRARIES}
		)

	endif()
	
	Message(STATUS "__#FOUND GLES2 ${OGRE_BUILD_RENDERSYSTEM_GLES2}")
	
endmacro(configure_rendersystem)

