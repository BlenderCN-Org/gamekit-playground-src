cmake_minimum_required(VERSION 2.6)

project (protocolBuffers)

add_custom_command( 
    OUTPUT whatever.h
    COMMAND ./getit.sh ${CMAKE_CURRENT_BINARY_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "build google buffers"
)

add_library(proto_dummy dummy.cpp whatever.h)