cmake_minimum_required(VERSION 2.6)

message(STATUS "DefaultAddon-Includes: ${OGREKIT_INCLUDE}")

include_directories(
    ${OGREKIT_INCLUDE}
    # ${CMAKE_CURRENT_SOURCE_DIR}/generated/protobuf
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR/Engine}
)

# macro to create the include-file for the embedded lua-scripts (that are generated into gsTemplates.h)
macro (write_luainclude_file filepath )

    message(STATUS "WRITE LUA INCLUDE: GDIR ${GAME_DIR}")

    file(WRITE ${filepath} "void install_Templates_DefaultAddon(lua_State* L)\n")
    file(APPEND ${filepath} "{\n")
    file(APPEND ${filepath} "    lua_pushvalue(L, LUA_GLOBALSINDEX);\n")
    
    # add the files
    file (GLOB_RECURSE LUA_FILES 
                "${GAME_DIR}/lua/*.lua")
                
    message(STATUS "WRITE LUA INCLUDE 2: ${LUA_FILES}")
                
    foreach(LUA_FILE ${LUA_FILES})
        get_filename_component(OUT1 ${LUA_FILE} NAME)
        #STRING(REPLACE "${GAME_DIR}/lua/" "" OUT1 ${LUA_FILE})
        STRING(REPLACE ".lua" "" OUT2 ${OUT1})
        STRING(TOUPPER "${OUT2}" UP_LUA)
        message (STATUS "embedded-lua: add ${UP_LUA}")
        
        file(APPEND ${filepath} "    luaL_dostring(L, ${UP_LUA}); \n")
    endforeach()
    
    file(APPEND ${filepath} "    lua_pop(L, 1); \n")
    file(APPEND ${filepath} "}\n")

endmacro()


######################## PROTOBUF (DISABLED FOR NOW)######################################################
#if (PROTOBUF_PROTOC_EXECUTABLE)
#    # compile test.proto to cpp and lua (TRUE)
#    WRAP_PROTO(PROTOC_GEN_FILES ${CMAKE_CURRENT_SOURCE_DIR}/generated/protobuf 0 test.proto)
#    message(STATUS "Found protoc! lets give it a try! Files:${PROTOC_GEN_FILES}")
#else()
#    message(STATUS "Couldn't find protoc-exe! let's take the pre-gererated stuff")
#    #untested yet
#    set (PROTOC_GEN_FILES 
#            generated/protobuf/lua-protobuf.cc
#            generated/protobuf/test.pb.cc
#            #generated/protobuf/test.pb-lua.cc
#    )
#     
#endif()
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

#####################
#   Embed LUA
#####################

    set(GAME_DIR "${CMAKE_CURRENT_SOURCE_DIR}" CACHE PATH "")

    write_luainclude_file(${CMAKE_CURRENT_SOURCE_DIR}/generated/embedded_lua/embeddedLua.inc ${GAME_DIR})

    file (GLOB_RECURSE LUA_FILES 
            "${GAME_DIR}/lua/*.lua")
            
    message(STATUS "Include following lua-files: ${LUA_FILES}")
            
    add_templates(0 GEN_TEMPLATES_OUT generated/embedded_lua/gsTemplates.h ${LUA_FILES})

    message(STATUS "GEN_OUT ${GEN_TEMPLATES_OUT}")
#######################


set (FILES 

    DefaultGameAddon.cpp
    
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/embedded_lua/embeddedLua.inc
    
#    ${PROTOC_GEN_FILES}

    ${GEN_TEMPLATES_OUT}
    
    ${SWIG_FILES}
    
    script/ttScript.cpp
    
    ${LUA_FILES}
)



message(STATUS "ADDONS ${ADDONS} Files:${FILES}")

add_library(DefaultGameAddon ${FILES})

#add this as addon to compiled with the runtimes
add_addon(DefaultGameAddon ${CMAKE_CURRENT_SOURCE_DIR})
