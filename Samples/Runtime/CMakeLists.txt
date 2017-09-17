# ---------------------------------------------------------
cmake_minimum_required(VERSION 2.6)

macro(set_output_location INTERNALPATH)
        # copy executable to runtime if wanted
        if (PLAYGROUND_EXPORT AND NOT ${PLAYGROUND_PATH} STREQUAL "") 
            set_target_properties(AppOgreKit PROPERTIES
                          RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PLAYGROUND_PATH}/${INTERNALPATH}
                          RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PLAYGROUND_PATH}/${INTERNALPATH}
            )    
        endif()
endmacro(set_output_location)

if (APPLE)
	include_directories(${OGRELITE_SOURCE_DIR}/OgreMain/include/OSX)
	SET(EXETYPE MACOSX_BUNDLE)
	SET_SOURCE_FILES_PROPERTIES( MainMenu.nib  PROPERTIES MACOSX_PACKAGE_LOCATION Resources )
	SET_SOURCE_FILES_PROPERTIES(momo_ogre.blend PROPERTIES MACOSX_PACKAGE_LOCATION Resources )
	SET (NibFile MainMenu.nib)
	link_libraries(${COCOA_LIBRARY})
	link_libraries(${CARBON_LIBRARY})
	link_libraries(${IOKIT_LIBRARY})
endif()

set(SRC 
	momo_ogre.blend
	Main.cpp
)



if (MSVC)
	message(STATUS "Set msvc openssl link-directory!")
	link_directories(${CMAKE_SOURCE_DIR}/Dependencies/Win32/openssl/vc/lib)
endif()

# custom nodes
if (NOT "${RUNTIME_BLENDERNODES}" STREQUAL "")
	subdirs(${RUNTIME_BLENDERNODES})
	include_directories(
		${RUNTIME_BLENDERNODES}/src-gen/Generated
		${RUNTIME_BLENDERNODES}/src-nodes/NodesImpl
	)
link_libraries(nodetree_Gamekit)
endif()
####

include_directories(
	${OGREKIT_INCLUDE}
	../../Dependencies/Source/tclap/include
)

message (STATUS "RUNTIME-ADDONS: ${ADDONS}")


#-------------ADDONS----------

#create addon-inclusion-file
write_addon_file("${CMAKE_CURRENT_SOURCE_DIR}/Addons.h")

# include the addon-source-directories
foreach(addonpath ${ADDONS_PATH})
    include_directories(${addonpath})
    message(STATUS "Include ${addonpath}")
endforeach()

# add link-libraries 
foreach(addon ${ADDONS})
    link_libraries(${addon})
    message(STATUS "add_lib ${addon}")
endforeach()


list(APPEND SRC Addons.h)

MESSAGE(STATUS "")

link_libraries(
   # /home/ttrocha/_dev/extprojects/incubation/thrift/lib/cpp/t/libthrift.a
	${OGREKIT_LIB} 
	pthread
	
)
#-----------------------------


set(HiddenCMakeLists ../CMakeLists.txt)
source_group(ParentCMakeLists FILES ${HiddenCMakeLists})

if (WIN32)
	set(PLAT_RESOURCE Win32/app.rc)
	set(RESOURCE )	
	set_source_files_properties(${RESOURCE} PROPERTIES HEADER_FILE_ONLY 1)
	source_group(Content\\Resources FILES ${PLAT_RESOURCE} ${RESOURCE})

	list(APPEND SRC ${PLAT_RESOURCE})
endif()



if (APPLE)

	add_executable(
		AppOgreKit 
		MACOSX_BUNDLE
		MainMenu.nib
		${SRC} 
		${HiddenCMakeLists}
	)

    # if playground_export is active change default output location
    # TODO: Still to be tested        
    set_output_location("runtimes/osx")


else ()

    if (MINGW)
        add_executable(AppOgreKit WIN32 ${SRC} ${HiddenCMakeLists})
        message(STATUS "USING WINAPI!!!!")
        # if playground_export is active change default output location        
        set_output_location("runtimes/windows")
    else()
        add_executable(AppOgreKit ${SRC} ${HiddenCMakeLists})
        # if playground_export is active change default output location        
        set_output_location("runtimes/linux")
    endif()
	
	IF (NOT INTERNAL_CREATE_DISTRIBUTABLE_MSVC_PROJECTFILES)
		ADD_CUSTOM_COMMAND(
			TARGET AppOgreKit 
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} ARGS -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/momo_ogre.blend ${CMAKE_CURRENT_BINARY_DIR}/momo_ogre.blend
		)
	ENDIF ()

endif()

