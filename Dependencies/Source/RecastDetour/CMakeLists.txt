# ---------------------------------------------------------
cmake_minimum_required(VERSION 2.6)

project(Recast)
SET(${PROJECT_NAME} 1.0)

set (recastfolder ${GAMEKIT_ADDITIONAL_DEP_FOLDER}/recastnavigation)

set(RECAST_SRC 
	${recastfolder}/Recast/Source/Recast.cpp
	${recastfolder}/Recast/Source/RecastAlloc.cpp
	${recastfolder}/Recast/Source/RecastArea.cpp
	${recastfolder}/Recast/Source/RecastContour.cpp
	${recastfolder}/Recast/Source/RecastFilter.cpp
	${recastfolder}/Recast/Source/RecastLayers.cpp
	${recastfolder}/Recast/Source/RecastMesh.cpp
	${recastfolder}/Recast/Source/RecastMeshDetail.cpp
	${recastfolder}/Recast/Source/RecastRasterization.cpp
	${recastfolder}/Recast/Source/RecastRegion.cpp
 	${recastfolder}/Recast/Source/RecastAssert.cpp
	${recastfolder}/RecastDemo/Source/PerfTimer.cpp
	${recastfolder}/RecastDemo/Source/ValueHistory.cpp
)

set(RECAST_HDR
	${recastfolder}/Recast/Include/Recast.h
	${recastfolder}/Recast/Include/RecastAlloc.h
	${recastfolder}/Recast/Include/RecastAssert.h
	
	${recastfolder}/RecastDemo/Include/PerfTimer.h	
	${recastfolder}/RecastDemo/Include/ValueHistory.h	
)

set(DEBUG_UTILS_SRC
	${recastfolder}/DebugUtils/Source/DebugDraw.cpp
	${recastfolder}/DebugUtils/Source/DetourDebugDraw.cpp
	${recastfolder}/DebugUtils/Source/RecastDebugDraw.cpp
	${recastfolder}/DebugUtils/Source/RecastDump.cpp
)

set(DEBUG_UTILS_HDR
	${recastfolder}/DebugUtils/Include/DebugDraw.h
	${recastfolder}/DebugUtils/Include/DetourDebugDraw.h
	${recastfolder}/DebugUtils/Include/RecastDebugDraw.h
	${recastfolder}/DebugUtils/Include/RecastDump.h
)


set(DETOUR_HDR
	${recastfolder}/Detour/Include/DetourAlloc.h
	${recastfolder}/Detour/Include/DetourAssert.h
	${recastfolder}/Detour/Include/DetourCommon.h
	${recastfolder}/Detour/Include/DetourMath.h
	${recastfolder}/Detour/Include/DetourNavMesh.h
	${recastfolder}/Detour/Include/DetourNavMeshBuilder.h
	${recastfolder}/Detour/Include/DetourNavMeshQuery.h
	${recastfolder}/Detour/Include/DetourNode.h
	${recastfolder}/Detour/Include/DetourStatus.h
	
	${recastfolder}/DetourTileCache/Include/DetourTileCache.h
	${recastfolder}/DetourTileCache/Include/DetourTileCacheBuilder.h
)

set(DETOUR_SRC
	${recastfolder}/Detour/Source/DetourAlloc.cpp
	${recastfolder}/Detour/Source/DetourCommon.cpp
	${recastfolder}/Detour/Source/DetourNavMesh.cpp
	${recastfolder}/Detour/Source/DetourNavMeshBuilder.cpp
	${recastfolder}/Detour/Source/DetourNavMeshQuery.cpp
	${recastfolder}/Detour/Source/DetourNode.cpp	
	${recastfolder}/Detour/Source/DetourAssert.cpp
	${recastfolder}/DetourTileCache/Source/DetourTileCache.cpp
	${recastfolder}/DetourTileCache/Source/DetourTileCacheBuilder.cpp
)

set(CROWD_HDR
${recastfolder}/DetourCrowd/Include/DetourCrowd.h
${recastfolder}/DetourCrowd/Include/DetourLocalBoundary.h
${recastfolder}/DetourCrowd/Include/DetourObstacleAvoidance.h
${recastfolder}/DetourCrowd/Include/DetourPathCorridor.h
${recastfolder}/DetourCrowd/Include/DetourPathQueue.h
${recastfolder}/DetourCrowd/Include/DetourProximityGrid.h
)

set(CROWD_SRC
${recastfolder}/DetourCrowd/Source/DetourCrowd.cpp
${recastfolder}/DetourCrowd/Source/DetourLocalBoundary.cpp
${recastfolder}/DetourCrowd/Source/DetourObstacleAvoidance.cpp
${recastfolder}/DetourCrowd/Source/DetourPathCorridor.cpp
${recastfolder}/DetourCrowd/Source/DetourPathQueue.cpp
${recastfolder}/DetourCrowd/Source/DetourProximityGrid.cpp
)


include_directories( ${recastfolder}/Recast/Include )
include_directories( ${recastfolder}/Detour/Include )
include_directories( ${recastfolder}/DebugUtils/Include )
include_directories( ${recastfolder}/DetourTileCache/Include )
include_directories( ${recastfolder}/DetourCrowd/Include )
include_directories( ${recastfolder}/RecastDemo/Include )

add_library(${PROJECT_NAME} 
				${RECAST_HDR} ${RECAST_SRC} 
				${DEBUG_UTILS_HDR} ${DEBUG_UTILS_SRC} 
				${DETOUR_HDR} ${DETOUR_SRC}
				${CROWD_HDR} ${CROWD_SRC})




config_target_if_iphone_build(${PROJECT_NAME}  TRUE)
