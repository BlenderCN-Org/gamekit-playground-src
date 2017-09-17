# ---------------------------------------------------------
cmake_minimum_required(VERSION 2.6)

PROJECT(bbkit)

SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")

include_directories(
	${OGREKIT_INCLUDE}
)

#	GLESv2

set (SRC	
				main.cpp
	gkBlackberryGame.cpp
)

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


link_directories(/home/ttrocha/_dev/_libs/blackberry/bbndk2.1/target/qnx6/x86/usr/lib)

link_libraries(
	${OGREKIT_LIB}	
	GLESv2
	bps 
	screen 
	EGL 
	m
	OpenAL
)
include_directories(
	$ENV{QNX_TARGET}/usr/include
)

if( "${BLACKBERRY_ARCHITECTURE}" STREQUAL "arm" )
 #link_libraries($ENV{QNX_TARGET}/armle-v7/usr/lib/libOpenAL.so)
else()
 #link_libraries($ENV{QNX_TARGET}/x86/usr/lib/libOpenAL.so)
endif()

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

set(HiddenCMakeLists ../CMakeLists.txt)
source_group(ParentCMakeLists FILES ${HiddenCMakeLists})


add_executable(${PROJECT_NAME}
	${SRC}
)

# copy executable to runtime if wanted
if (PLAYGROUND_EXPORT AND NOT ${PLAYGROUND_PATH} STREQUAL "") 
    if (BB_PLAYBOOK)
    	MESSAGE(STATUS "PLAYGROUND-EXPORT FOR BB-PLAYBOOK")
	    set_target_properties(${PROJECT_NAME} PROPERTIES
	                  RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PLAYGROUND_PATH}/runtimes/blackberry-playbook
	                  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PLAYGROUND_PATH}/runtimes/blackberry-playbook
	    )
	else()
    	MESSAGE(STATUS "PLAYGROUND-EXPORT FOR BB-BB10")
	    set_target_properties(${PROJECT_NAME} PROPERTIES
	                  RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PLAYGROUND_PATH}/runtimes/blackberry-bb10
	                  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PLAYGROUND_PATH}/runtimes/blackberry-bb10
	    )
	endif()    
endif()

if (1) #cp && strip

set (OUTPUT_NAME ${CMAKE_SHARED_LIBRARY_PREFIX}${PROJECT_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}) #lib<name>.so
set (STRIP_TOOL arm-linux-androideabi-strip)

message(STATUS "From ${CMAKE_CURRENT_SOURCE_DIR}/../Demo/libs/${ANDROID_ABI}/${OUTPUT_NAME} TO:${OUTPUT_NAME}")


endif()

set(LIBRARY_NAME ${PROJECT_NAME})

