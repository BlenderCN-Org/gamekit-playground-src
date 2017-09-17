cmake_minimum_required(VERSION 2.6)



include_directories(
    ${OGREKIT_INCLUDE}
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/protobuf
    ${CMAKE_CURRENT_SOURCE_DIR}
)


######################## PROTOBUF ######################################################
if (PROTOBUF_PROTOC_EXECUTABLE)
    # compile test.proto to cpp and lua (TRUE)
    WRAP_PROTO(PROTOC_GEN_FILES ${CMAKE_CURRENT_SOURCE_DIR}/generated/protobuf 0 test.proto)
    message(STATUS "Found protoc! lets give it a try! Files:${PROTOC_GEN_FILES}")
else()
    message(STATUS "Couldn't find protoc-exe! let's take the pre-gererated stuff")
    #untested yet
    set (PROTOC_GEN_FILES 
            generated/protobuf/lua-protobuf.cc
            generated/protobuf/test.pb.cc
            #generated/protobuf/test.pb-lua.cc
    )
     
endif()
###########################################################################################


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
#use the pre-generated stuff    
endif()
##########################################################################################

set (FILES 
    
    ${PROTOC_GEN_FILES}
    ${SWIG_FILES}
    
    script/ttScript.cpp
    
    GameServer.cpp
    GameAddon.cpp
)

message(STATUS "ADDONS ${ADDONS} Files:${FILES}")

add_library(GameAddon ${FILES})

#add this as addon to compiled with the runtimes
add_addon(GameAddon ${CMAKE_CURRENT_SOURCE_DIR})
