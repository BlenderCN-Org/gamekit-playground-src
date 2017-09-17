cmake_minimum_required(VERSION 2.6)

project(
##name|pre#nodetree_:treename##
nodetree_templates
##endname##
)

MESSAGE(STATUS "Generated Logic: ${CMAKE_CURRENT_SOURCE_DIR}")
MESSAGE(STATUS "gkOgreKITInclude: ${OGREKIT_INCLUDE}")

include_directories(
	${OGREKIT_INCLUDE}
##block:include##
	##name:includeDir##
	src-nodes/NodesImpl
	##endname##
##endblock:includeDir##
##block:rip##
	src-gen/Generated
##endblock:rip##
)

set(
	FILES
##block:file##
	##name:fileName##	
	gkAbstractNodesTemplate.h
	##endname##
##endblock:file##
##block:rip##	
	gkAbstractNodesTemplate.cpp
	gkTemplateNode.h
	gkTemplateNode.cpp
	
	gsAbstractNodesTemplate.h
	gsAbstractNodesTemplate.cpp
	
##endblock:rip##
)

add_library(${PROJECT_NAME} ${FILES})