PROJECT(skyx)   

SET(SKYX_HRDS
    include/AtmosphereManager.h
    include/BasicController.h
    include/CloudsManager.h
    include/ColorGradient.h
    include/Controller.h
    include/GPUManager.h
    include/MeshManager.h
    include/MoonManager.h
    include/Prerequisites.h
    include/SkyX.h
    include/VCloudsManager.h
    include/VClouds/DataManager.h
    include/VClouds/Ellipsoid.h
    include/VClouds/FastFakeRandom.h
    include/VClouds/GeometryBlock.h
    include/VClouds/GeometryManager.h
    include/VClouds/Lightning.h
    include/VClouds/LightningManager.h
    include/VClouds/VClouds.h  )

SET(SKYX_SRCS
    src/AtmosphereManager.cpp
    src/BasicController.cpp
    src/CloudsManager.cpp
    src/ColorGradient.cpp
    src/GPUManager.cpp
    src/MeshManager.cpp
    src/MoonManager.cpp
    src/Prerequisites.cpp
    src/SkyX.cpp
    src/VCloudsManager.cpp
    src/VClouds/DataManager.cpp
    src/VClouds/Ellipsoid.cpp
    src/VClouds/FastFakeRandom.cpp
    src/VClouds/GeometryBlock.cpp
    src/VClouds/GeometryManager.cpp
    src/VClouds/Lightning.cpp
    src/VClouds/LightningManager.cpp
    src/VClouds/VClouds.cpp)
   
include_directories(
    include
#    ${Boost_INCLUDE_DIRS}
)
ADD_LIBRARY(skyx ${SKYX_SRCS} ${SKYX_HRDS})