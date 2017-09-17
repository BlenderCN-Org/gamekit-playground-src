cmake_minimum_required(VERSION 2.6)

set (thrift_folder "${GAMEKIT_ADDITIONAL_DEP_FOLDER}/thrift")

include_directories(
    ${OGREKIT_INCLUDE}
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/protobuf
    ${CMAKE_CURRENT_SOURCE_DIR}

)

if (UNIX)
    include_directories(
        ${CMAKE_SOURCE_DIR}/Dependencies/Source/thrift/src/unix
    )
endif()

set (THRIFTFILE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set (THRIFTFILE_NAME gameserver)
set (THRIFT_OUTDIR ${CMAKE_CURRENT_SOURCE_DIR}/generated)
set (SERVICE_PREFIX TGameServerService)

add_thrift(${THRIFTFILE_DIR} ${THRIFTFILE_NAME} ${THRIFT_OUTDIR} ${SERVICE_PREFIX}) 





######################### SWIG SCRIPT #####################################################
if (OGREKIT_COMPILE_SWIG AND SWIG_FOUND)
    set (args "-lua" "-c++")
    
    set(Script_HEADER
        script/ttScript.h
    )
    
    swig_source_language("Lua.cpp" ../generated/script args SWIG_API2_OUTPUT_LUA Script_HEADER script/ttScript.i)
#    swig_source_lua(SWIG_API2_OUTPUT_LUA Script_HEADER script/ttScript.i)
    message(STATUS "AFTER SWIG: ${SWIG_API2_OUTPUT_LUA} header: ${Script_HEADER}")

    set(SWIG_FILES ${SWIG_API2_OUTPUT_LUA})
else()
    set(SWIG_FILES generated/script/ttScriptLua.cpp)
#use the pre-generated stuff    
endif()
##########################################################################################

message(STATUS "ADDON SWIG:${SWIG_FILES}")

set (FILES 
    

    ${SWIG_FILES}
    
    script/ttScript.cpp
    
    #generated/gen-cpp/TGameServerService_server.skeleton.cpp
    generated/gen-cpp/TGameServerService.cpp
    generated/gen-cpp/TGameServerService.h
    generated/gen-cpp/gameserver_constants.cpp
    generated/gen-cpp/gameserver_constants.h
    generated/gen-cpp/gameserver_types.cpp
    generated/gen-cpp/gameserver_types.h
    
    gkThriftManager.cpp
    gkThriftManager.h
    
    ThriftAddon.cpp
)



message(STATUS "ADDONS ${ADDONS} Files:${FILES}")

include_directories(${CMAKE_SOURCE_DIR}/Dependencies/Source/openssl/lib/include)

add_definitions(-DHAVE_CONFIG_H )

if (MSVC)
	include_directories(${CMAKE_SOURCE_DIR}/Dependencies/Source/thrift/src 
						${CMAKE_SOURCE_DIR}/Dependencies/Source/thrift/src/thrift
						${CMAKE_SOURCE_DIR}/Dependencies/Source/thrift/src/thrift/windows)
else()
    message(STATUS "____thriftaddon: ${thrift_folder}")
    include_directories(${thrift_folder}/git/lib/cpp/src 
                        ${thrift_folder}/git/lib/cpp/src/thrift
                        ${CMAKE_SOURCE_DIR}/Dependencies/Source/thrift/config/unix)
endif()

link_libraries(thrift)

message(STATUS "Thrift-FILES: ${FILES}" )
add_library(ThriftAddon ${FILES})

#add this as addon to compiled with the runtimes
add_addon(ThriftAddon ${CMAKE_CURRENT_SOURCE_DIR})
