# Try to find protocol buffers (protobuf)
#
# Use as FIND_PACKAGE(ProtocolBuffers)
#
#  PROTOBUF_FOUND - system has the protocol buffers library
#  PROTOBUF_INCLUDE_DIR - the zip include directory
#  PROTOBUF_LIBRARY - Link this to use the zip library
#  PROTOBUF_PROTOC_EXECUTABLE - executable protobuf compiler
#
# And the following command
#
#  WRAP_PROTO(VAR input1 input2 input3..)
#
# Which will run protoc on the input files and set VAR to the names of the created .cc files,
# ready to be added to ADD_EXECUTABLE/ADD_LIBRARY. E.g,
#
#  WRAP_PROTO(PROTO_SRC myproto.proto external.proto)
#  ADD_EXECUTABLE(server ${server_SRC} {PROTO_SRC})
#
# Author: Esben Mose Hansen <esben at ange.dk>, (C) Ange Optimization ApS 2008
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

IF (PROTOBUF_LIBRARY AND PROTOBUF_INCLUDE_DIR AND PROTOBUF_PROTOC_EXECUTABLE)
  # in cache already
  SET(PROTOBUF_FOUND TRUE)
ELSE (PROTOBUF_LIBRARY AND PROTOBUF_INCLUDE_DIR AND PROTOBUF_PROTOC_EXECUTABLE)

  FIND_PATH(PROTOBUF_INCLUDE_DIR stubs/common.h
    /usr/include/google/protobuf
    /usr/local/include/google/protobuf
  )

  FIND_LIBRARY(PROTOBUF_LIBRARY NAMES protobuf
    PATHS
    ${GNUWIN32_DIR}/lib
    /usr/local/lib
    
  )

  FIND_PROGRAM(PROTOBUF_PROTOC_EXECUTABLE protoc)

  INCLUDE(FindPackageHandleStandardArgs)
  FIND_PACKAGE_HANDLE_STANDARD_ARGS(protobuf DEFAULT_MSG PROTOBUF_INCLUDE_DIR PROTOBUF_LIBRARY PROTOBUF_PROTOC_EXECUTABLE)

  # ensure that they are cached
  SET(PROTOBUF_INCLUDE_DIR ${PROTOBUF_INCLUDE_DIR} CACHE INTERNAL "The protocol buffers include path")
  SET(PROTOBUF_LIBRARY ${PROTOBUF_LIBRARY} CACHE INTERNAL "The libraries needed to use protocol buffers library")
  SET(PROTOBUF_PROTOC_EXECUTABLE ${PROTOBUF_PROTOC_EXECUTABLE} CACHE INTERNAL "The protocol buffers compiler")

ENDIF (PROTOBUF_LIBRARY AND PROTOBUF_INCLUDE_DIR AND PROTOBUF_PROTOC_EXECUTABLE)

IF (PROTOBUF_FOUND)
  # Define the WRAP_PROTO function
  FUNCTION(WRAP_PROTO VAR OUTPUT_FOLDER USE_LUA)

    
    IF (NOT ARGN)
      MESSAGE(SEND_ERROR "Error: WRAP PROTO called without any proto files")
      RETURN()
    ENDIF(NOT ARGN)

    if (NOT OUTPUT_FOLDER)
        SET(OUTPUT_FOLDER ${CMAKE_CURRENT_BINARY_DIR})
    endif() 

    SET (LUA_COMMONS_ADDED)
    SET(INCL)
    SET(${VAR})
    FOREACH(FIL ${ARGN})
      GET_FILENAME_COMPONENT(ABS_FIL ${FIL} ABSOLUTE)
      GET_FILENAME_COMPONENT(FIL_WE ${FIL} NAME_WE)
      LIST(APPEND ${VAR} "${OUTPUT_FOLDER}/${FIL_WE}.pb.cc")
      LIST(APPEND INCL "${OUTPUT_FOLDER}/${FIL_WE}.pb.h")

        

    if(USE_LUA)
      LIST(APPEND ${VAR} ${OUTPUT_FOLDER}/${FIL_WE}.pb-lua.cc ${OUTPUT_FOLDER}/lua-protobuf.cc)
      LIST(APPEND INCL ${OUTPUT_FOLDER}/${FIL_WE}.pb-lua.h ${OUTPUT_FOLDER}/lua-protobuf.h)
    endif()

      ADD_CUSTOM_COMMAND(
        OUTPUT ${OUTPUT_FOLDER}/${FIL_WE}.pb.cc ${OUTPUT_FOLDER}/${FIL_WE}.pb.h 
        COMMAND  ${PROTOBUF_PROTOC_EXECUTABLE}
        ARGS --cpp_out  ${OUTPUT_FOLDER} --proto_path ${CMAKE_CURRENT_SOURCE_DIR} ${ABS_FIL}
        DEPENDS ${ABS_FIL}
        COMMENT "Running protocol buffer compiler on ${FIL}"
        VERBATIM )
        
      if(USE_LUA)
      message(STATUS "protoc use-lua:${USE_LUA} output to ${OUTPUT_FOLDER} ")

          set(LUA_COMMONS)
          #if first time called we need to also add the generated lua-protobuf.* files to the custom-command
          if(NOT LUA_COMMONS_ADDED)
            set(LUA_COMMONS ${OUTPUT_FOLDER}/lua-protobuf.cc ${OUTPUT_FOLDER}/lua-protobuf.h)
            set(LUA_AFTER_ONE 1)
          endif()
          
          ADD_CUSTOM_COMMAND(
            OUTPUT ${OUTPUT_FOLDER}/${FIL_WE}.pb-lua.cc ${OUTPUT_FOLDER}/${FIL_WE}.pb-lua.h ${LUA_COMMONS}
            COMMAND  ${PROTOBUF_PROTOC_EXECUTABLE}
            ARGS --lua_out  ${OUTPUT_FOLDER} --proto_path ${CMAKE_CURRENT_SOURCE_DIR} ${ABS_FIL}
            DEPENDS ${ABS_FIL}
            COMMENT "Running protocol buffer compiler on ${FIL}"
            VERBATIM )
          
          
       endif() 
    ENDFOREACH(FIL)

    SET(${VAR} ${${VAR}} PARENT_SCOPE)

  ENDFUNCTION(WRAP_PROTO)
ENDIF(PROTOBUF_FOUND)
