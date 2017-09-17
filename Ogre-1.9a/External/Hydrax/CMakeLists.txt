PROJECT(Hydrax)

SET(HYDRAX_HRDS
    include/CfgFileManager.h
    include/DecalsManager.h
    include/Enums.h
    include/GodRaysManager.h
    include/GPUNormalMapManager.h
    include/Help.h
    include/Hydrax.h
    include/Image.h
    include/MaterialManager.h
    include/Mesh.h
    include/Prerequisites.h
    include/RttManager.h
    include/TextureManager.h
    include/Modules/Module.h
    include/Modules/ProjectedGrid/ProjectedGrid.h
    include/Modules/RadialGrid/RadialGrid.h
    include/Modules/SimpleGrid/SimpleGrid.h
    include/Noise/Noise.h
    include/Noise/FFT/FFT.h
    include/Noise/Perlin/Perlin.h)

SET(HYDRAX_SRCS
    src/CfgFileManager.cpp
    src/DecalsManager.cpp
    src/Enums.cpp
    src/GodRaysManager.cpp
    src/GPUNormalMapManager.cpp
    src/Help.cpp
    src/Hydrax.cpp
    src/Image.cpp
    src/MaterialManager.cpp
    src/Mesh.cpp
    src/Prerequisites.cpp
    src/RttManager.cpp
    src/TextureManager.cpp
    src/Modules/Module.cpp
    src/Modules/ProjectedGrid/ProjectedGrid.cpp
    src/Modules/RadialGrid/RadialGrid.cpp
    src/Modules/SimpleGrid/SimpleGrid.cpp
    src/Noise/Noise.cpp
    src/Noise/FFT/FFT.cpp
    src/Noise/Perlin/Perlin.cpp)

include_directories(
#    ${OGREKIT_OGRE_INCLUDE}
#    ${Boost_INCLUDE_DIRS}
    include
    include/Modules
    include/Modules/ProjectedGrid
    include/Modules/RadialGrid
    include/Modules/SimpleGrid
    include/Noise
    include/Noise/FFT
    include/Noise/Perlin
)

ADD_LIBRARY(hydrax ${HYDRAX_SRCS} ${HYDRAX_HRDS})
