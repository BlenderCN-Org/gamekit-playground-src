project (gknet)

set (NET_CPP	
	gkNetSerialize.cpp
	gkNetworkServerThread.cpp
	gkMessageNetForwarder.cpp
	gkNetSerialize.cpp
)

set (NET_HDR	gkNetworkManager.h
				gkNetSerialize.h
				gkNetworkServerThread.h
				gkMessageNetForwarder.h
)

set (OGREKIT_NETWORKMGR_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR} ${ENET_INCLUDE_DIR} CACHE INTERNAL "")
set (OGREKIT_NETWORKMGR_LIB ${PROJECT_NAME} CACHE INTERNAL "")

if (OGREKIT_NETWORK_ENET_SUPPORT)
	list(APPEND NET_CPP
		Enet/gkENetNetworkManager.cpp
	)	

	include_directories(${ENET_INCLUDE_DIR})

	link_libraries (ENET)
	
endif()


if (OGREKIT_COMPILE_LIBCURL) 
   link_libraries(curl)
   list(APPEND NET_HDR
            gkDownloadManager.h
   )
   list(APPEND NET_CPP
            gkDownloadManager.cpp
   )
           
endif()

include_directories( ${OGREKIT_INCLUDE} )


add_library( ${PROJECT_NAME} ${NET_CPP} ${NET_HDR} )
