# ---------------------------------------------------------
cmake_minimum_required(VERSION 2.6)

PROJECT(bbkit)

SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")

add_definitions(-Wno-warn-absolute-paths)

include_directories(
	${OGREKIT_INCLUDE}
	EGL
	GLESv2
)

#	GLESv2

set (SRC	
	main.cpp
	gkEmscriptenGame.cpp
)


link_libraries(
	${OGREKIT_LIB}	
)

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


set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

set(HiddenCMakeLists ../CMakeLists.txt)
source_group(ParentCMakeLists FILES ${HiddenCMakeLists})

if ( NOT ("${OGREKIT_EMSCRIPTEN_VIRTUAL_DIR}"  STREQUAL "") )
	MESSAGE (STATUS "EM VIRTUAL DIR:${OGREKIT_EMSCRIPTEN_VIRTUAL_DIR}")
	SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} --preload-file ${OGREKIT_EMSCRIPTEN_VIRTUAL_DIR}/@." )
endif()

if (OGREKIT_EMSCRIPTEN_OUTPUT_HTMLFILE) 
	SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -o ${PROJECT_NAME}.html" )
	MESSAGE (STATUS "Output to ==> ${PROJECT_NAME}.html")
endif()

SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s ALLOW_MEMORY_GROWTH=1 -s ALLOW_MEMORY_GROWTH=1 --llvm-opts 3  -s DISABLE_EXCEPTION_CATCHING=2  -s DEMANGLE_SUPPORT=1 -s ASSERTIONS=2" )


add_executable(${PROJECT_NAME}
	${SRC}
)


# copy executable to runtime if wanted
if (PLAYGROUND_EXPORT AND NOT ${PLAYGROUND_PATH} STREQUAL "") 
    set_target_properties(${PROJECT_NAME} PROPERTIES
                  RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PLAYGROUND_PATH}/runtimes/web
                  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PLAYGROUND_PATH}/runtimes/web
    )    
endif()