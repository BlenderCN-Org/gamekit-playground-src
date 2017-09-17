# ---------------------------------------------------------
cmake_minimum_required(VERSION 2.6)

project(ENET)
SET(${PROJECT_NAME} 1.0)

set(ENET_SRC 
	src/callbacks.c
	src/compress.c
	src/host.c
	src/list.c
	src/packet.c
	src/peer.c
	src/protocol.c
	src/unix.c
	src/win32.c
	src/extra/serialize.c
)

set(ENET_HDR
	include/enet/callbacks.h
	include/enet/list.h
	include/enet/protocol.h
	include/enet/unix.h
	include/enet/win32.h
	include/enet/extra/serialize.h
)

if (WIN32)
	message (STATUS "ENet-Support Win32")
#	list(APPEND ENET_SRC
#				src/win32.c
#			)
#
#	list(APPEND ENET_HDR
#				include/win32.h
#			)
    link_libraries(winmm)
elseif (APPLE)
    message(STATUS "ENET APPLE!")
    add_definitions(-g -O2 -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 
                            -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 
                            -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_DLFCN_H=1 -DHAS_POLL=1 
                            -DHAS_FCNTL=1 -DHAS_INET_PTON=1 -DHAS_INET_NTOP=1 -DHAS_MSGHDR_FLAGS=1 
                            -DHAS_SOCKLEN_T=1)
else()
# took this definitions from the makefile! need to be checked for win and iOS
	add_definitions(-g -O2  -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 
	                   -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 
	                   -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 
	                   -DHAVE_DLFCN_H=1 -DLT_OBJDIR=\".libs/\" -DHAS_GETHOSTBYADDR_R=1  -DHAS_POLL=1 
	                   -DHAS_FCNTL=1 -DHAS_INET_PTON=1 -DHAS_INET_NTOP=1 -DHAS_MSGHDR_FLAGS=1 -DHAS_SOCKLEN_T=1)

	if (OGREKIT_BUILD_ANDROID )
		add_definitions( -DHAS_GETHOSTBYNAME_R=0 )
	else()
		add_definitions( -DHAS_GETHOSTBYNAME_R=1 )
	endif()
	message (STATUS "ENet-Support UNIX")
#	list(APPEND ENET_SRC
#				
#			)
#
#	list(APPEND ENET_HDR
#				include/unix.h
#			)
endif()

set (ENET_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE INTERNAL "")
message(STATUS "Enet include-dir: ${ENET_INCLUDE_DIR}")

include_directories( include  )

add_definitions (-fpermissive)

add_library(${PROJECT_NAME} ${ENET_HDR} ${ENET_SRC})

config_target_if_iphone_build(${PROJECT_NAME} TRUE)
