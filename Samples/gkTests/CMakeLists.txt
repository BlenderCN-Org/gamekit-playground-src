# ---------------------------------------------------------
cmake_minimum_required(VERSION 2.6)

project(gkTest)

file(GLOB_RECURSE DATA_SRC  data/*.blend)

MESSAGE(STATUS "___gkTest-Data-Files:${DATA_SRC}")

set(DEMO_SRC
	main.cpp
	${DATA_SRC}
)

set(ALL
	${DEMO_SRC}
	${DEMO_HDR}
)




include_directories(
	. 
	${OGREKIT_INCLUDE}
)

link_libraries(
	${OGREKIT_LIB}
)

#------------------copy-------------------------------
ADD_CUSTOM_TARGET(COPY_TARGET)
file( MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/data" )
foreach(file ${DATA_SRC})
    ADD_CUSTOM_COMMAND(
        TARGET COPY_TARGET 
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} ARGS -E copy_if_different ${file} ${CMAKE_CURRENT_BINARY_DIR}/data
    )
endforeach()

ADD_CUSTOM_COMMAND(
    TARGET COPY_TARGET 
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} ARGS -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/res.cfg ${CMAKE_CURRENT_BINARY_DIR}
)

#-----------------------------------------------------

add_executable(${PROJECT_NAME}  ${ALL} )

add_dependencies(${PROJECT_NAME} COPY_TARGET)


