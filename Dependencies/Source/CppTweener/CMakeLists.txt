# ---------------------------------------------------------
cmake_minimum_required(VERSION 2.6)

project(cpptweener)
SET(${PROJECT_NAME} 1.0)

set(FILES 
	src/CppTweener.cpp
    include/CppTweener.h
)

include_directories(include)

add_library(${PROJECT_NAME} ${FILES})

config_target_if_iphone_build(${PROJECT_NAME} TRUE)
