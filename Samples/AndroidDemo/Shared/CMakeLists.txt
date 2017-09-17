# ---------------------------------------------------------
cmake_minimum_required(VERSION 2.6)

PROJECT(ogrekit)

set (thrift_folder "${GAMEKIT_ADDITIONAL_DEP_FOLDER}/thrift")

add_definitions(-lfl)

configure_file("${CMAKE_SOURCE_DIR}/CMake/Templates/Android.mk.in" "${CMAKE_CURRENT_SOURCE_DIR}/../Demo/jni/Android.mk" @ONLY)


include_directories(
	${OGREKIT_INCLUDE}
    ${thrift_folder}/git/lib/cpp/src
    ${thrift_folder}/git/lib/cpp/src/thrift/concurrency
    ${thrift_folder}/git/lib/cpp/src/thrift
    ${thrift_folder}/git/lib/cpp/src/thrift/transport
    ${thrift_folder}/git/lib/cpp/src/thrift/processor
    ${thrift_folder}/git/lib/cpp/src/thrift/server
    ${CMAKE_SOURCE_DIR}/Dependencies/Source/thrift/config/unix
)

message(STATUS "Thrift: ${CMAKE_SOURCE_DIR}/Dependencies/Source/thrift/config/unix")

add_definitions(-DHAVE_CONFIG_H=1 ) 

set (SRC  gkAndroidGame.cpp OgreActivityJNI.cpp 
)

link_libraries(
	${OGREKIT_LIB}	
	log 
	GLESv2
	EGL	
	android	
)


set(LIBRARY_OUTPUT_PATH .)

set(HiddenCMakeLists ../CMakeLists.txt)
source_group(ParentCMakeLists FILES ${HiddenCMakeLists})

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

link_libraries(
    ${OGREKIT_LIB} 
)
#-----------------------------


# custom nodes
if (NOT "${RUNTIME_BLENDERNODES}" STREQUAL "")
	subdirs(${RUNTIME_BLENDERNODES})
	include_directories(
		${RUNTIME_BLENDERNODES}/src-gen/Generated
		${RUNTIME_BLENDERNODES}/src-nodes/NodesImpl
	)
endif()

link_libraries(nodetree_Gamekit)
####


MESSAGE(STATUS "ANDROID include ${ANDROID_SYSROOT}/usr/include ${OGRELITE_SOURCE_DIR}/RenderSystems/GLES2/include/EGL   ${OGRELITE_SOURCE_DIR}/RenderSystems/GLES2/include")

include_directories(
	${ANDROID_SYSROOT}/usr/include ${OGRELITE_SOURCE_DIR}/RenderSystems/GLES2/include/EGL 
	${OGRELITE_SOURCE_DIR}/RenderSystems/GLES2/include
	${OGRELITE_SOURCE_DIR}/OgreMain/include/Android  
)

 

ADD_LIBRARY(${PROJECT_NAME}
	SHARED
    $ENV{NDK}/sources/android/cpufeatures/cpu-features.c
	${SRC}
)




    add_custom_command(
                        TARGET ${PROJECT_NAME}
                        POST_BUILD
                        COMMAND ant debug
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../Demo
                      )

    if (PLAYGROUND_EXPORT AND NOT ${PLAYGROUND_PATH} STREQUAL "") 
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/../Demo/libs/armeabi-v7a/libogrekit.so ${PLAYGROUND_PATH}/runtimes/android/libs/armeabi-v7a/libogrekit.so
        )
    endif()
set(LIBRARY_NAME ${PROJECT_NAME})
