cmake_minimum_required(VERSION 2.6)

project(blendload)

set(FILES
    gkBlenderDefines.h
	gkNodeTreeConverter.cpp
	gkNodeTreeConverter.h
    gkBlenderSceneConverter.cpp
    gkBlenderSceneConverter.h
    gkBlendFile.cpp
    gkBlendFile.h
    gkBlendInternalFile.cpp
    gkBlendInternalFile.h
    gkBlendLoader.cpp
    gkBlendLoader.h
    gkLoaderCommon.h
    gkTextureLoader.cpp
    gkTextureLoader.h
    
    Converters/gkAnimationConverter.cpp
    Converters/gkAnimationConverter.h
    Converters/gkLogicBrickConverter.cpp
    Converters/gkLogicBrickConverter.h
    Converters/gkMeshConverter.cpp
    Converters/gkMeshConverter.h
    Converters/gkParticleConverter.cpp
    Converters/gkParticleConverter.h
    Converters/gkSkeletonConverter.cpp
    Converters/gkSkeletonConverter.h
)

MESSAGE(STATUS "BLEND_INCLUDE ${FBT_BLEND_INCLUDE} fbt:${FBT_INCLUDE}")
include_directories(${FBT_BLEND_INCLUDE} ${FBT_INCLUDE})

add_library(${PROJECT_NAME} ${FILES})