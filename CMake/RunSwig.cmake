# -----------------------------------------------------------------------------
#
#  Compiletime generators for SWIG
#   http://www.swig.org/
#
# -----------------------------------------------------------------------------


  set (SWIG_EXECUTABLE /usr/local/bin/swig)
  set (SWIG_LIB_ROOT /usr/local/bin)

  if ((OGREKIT_BUILD_MOBILE OR (MINGW AND CMAKE_CROSSCOMPILING) OR OGREKIT_BUILD_EMSCRIPTEN) AND OGREKIT_COMPILE_SWIG)
	message(status "SWIG: manually set swig-exe to swig")
	set (SWIG_FOUND TRUE)
  else()
 	message(STATUS "SWIG: NOT OVERRIDDEN! Do it the old way")
	include(FindSWIG)
	set(SWIG_LIB_ROOT    ${SWIG_DIR})
  endif()

# -----------------------------------------------------------------------------
#
# Get a list of absolute paths
#
# -----------------------------------------------------------------------------
macro(SWIG_SOURCE_ABSOLUTE_SRC OUT)
    foreach (it ${ARGN})
        get_filename_component(N ${it} ABSOLUTE)
        set(${OUT} ${${OUT}} ${N})
    endforeach(it)
endmacro(SWIG_SOURCE_ABSOLUTE_SRC)
# -----------------------------------------------------------------------------
#
# Generic call
#
# For each ${INPUT_FILE}
#   Call ${SWIG} -o ${OUT_FILE} ${ARGUMENT_LIST} ${INPUT_FILE}
#
#
# -----------------------------------------------------------------------------
macro(SWIG_SOURCE_LANGUAGE LANG GENDIR ARGS OUT DEPS)

 

    if (SWIG_FOUND )
    

        swig_source_absolute_src(ABS_DEPS ${${DEPS}})
        set(SWIG_REAL_LIB
            ${SWIG_LIB_ROOT}/Lib
        )
        file(GLOB_RECURSE LIB_FILES ${SWIG_REAL_LIB}/*.swg ${SWIG_REAL_LIB}/*.i)

        foreach (ARG ${ARGN})
	    message(STATUS "SWIG: ${ARG}")

            get_filename_component(IFILE ${ARG} ABSOLUTE)
            string(REPLACE ".i" "${LANG}" OFILE ${IFILE})

            string(REPLACE ".cpp" "" BENAME ${LANG})

            get_filename_component(INAME ${IFILE} NAME)
            get_filename_component(ONAME ${OFILE} NAME)

            set(ROFILE ${OFILE})
            if (NOT ${GENDIR} STREQUAL "")
                get_filename_component(OPATH ${OFILE} PATH)
                set(ROFILE "${OPATH}/${GENDIR}/${ONAME}")
                set(RUNTIME_FILE "${OPATH}/${GENDIR}/swigluarun.h")
            endif()
            add_custom_command( 
                OUTPUT ${RUNTIME_FILE}
                COMMAND ${SWIG_EXECUTABLE} -c++ -lua -external-runtime ${RUNTIME_FILE}
                WORKING_DIRECTORY ${SWIG_LIB_ROOT}
                COMMENT "${SWIG_EXECUTABLE} external runtime creation..."
            )
    
            add_custom_command(
                OUTPUT  ${ROFILE}
                COMMAND ${SWIG_EXECUTABLE} -o ${ROFILE} ${${ARGS}} ${IFILE}
                DEPENDS ${SWIG_EXECUTABLE} ${IFILE} ${ABS_DEPS} ${LIB_FILES}
                WORKING_DIRECTORY ${SWIG_LIB_ROOT}
                COMMENT "${SWIG_EXECUTABLE} ${ONAME} -${BENAME} ${INAME}"
            )
            list(APPEND ${OUT} ${ROFILE})
        endforeach(ARG)

    endif()

    


endmacro(SWIG_SOURCE_LANGUAGE)
# -----------------------------------------------------------------------------
#
# Lua
#   http://www.lua.org/
#
# -----------------------------------------------------------------------------
macro(SWIG_SOURCE_LUA OUT DEPS)
	MESSAGE(STATUS "Swig Version:${SWIG_VERSION}")
    set(ARGS "-lua")
    #list(APPEND ARGS "-no-old-metatable-bindings")
    list(APPEND ARGS "-squash-bases")
    list(APPEND ARGS "-c++")
	list(APPEND ARGS "-I${RUNTIME_BLENDERNODES}/src-gen/Generated")
    if (OGREKIT_USE_PROCESSMANAGER)
	    list(APPEND ARGS "-DOGREKIT_USE_PROCESSMANAGER")
    endif()
    
    if (OGREKIT_COMPILE_RECAST)
        list(APPEND ARGS "-DOGREKIT_COMPILE_RECAST")
    endif()
    
    if (OGREKIT_COMPILE_OPENSTEER)
        list(APPEND ARGS "-DOGREKIT_COMPILE_OPENSTEER")
    endif()
    
    if (OGREKIT_COMPILE_LIBROCKET)
        list(APPEND ARGS "-DOGREKIT_COMPILE_LIBROCKET")
    endif()
	
	if (OGREKIT_BUILD_OGREPROCEDURAL)
        list(APPEND ARGS "-DOGREKIT_BUILD_OGREPROCEDURAL")
    endif()

    if (OGREKIT_COMPILE_GUI3D)
        list(APPEND ARGS "-DOGREKIT_COMPILE_GUI3D")
		list(APPEND ARGS "-I${GAMEKIT_ADDITIONAL_DEP_FOLDER}/gui3d/build/source")
    endif()

    if (OGREKIT_USE_THEORA)
        list(APPEND ARGS "-DOGREKIT_USE_THEORA")
    endif()

    

    swig_source_language("Lua.cpp" "Generated" ARGS ${OUT} ${DEPS} ${ARGN})
endmacro(SWIG_SOURCE_LUA)
# -----------------------------------------------------------------------------
#
# XML Tests
#
# -----------------------------------------------------------------------------
macro(SWIG_SOURCE_XML OUT DEPS)
    set(ARGS "-xml")
    list(APPEND ARGS "-c++")
    swig_source_language(".xml" "Generated" ARGS ${OUT} ${DEPS} ${ARGN})
endmacro(SWIG_SOURCE_XML)
# -----------------------------------------------------------------------------
#
# External XML Tests
#
# For each ${INPUT_FILE}
#   Call ${SWIG_XML} ${OUT_FILE} ${INPUT_FILE}
#
#
# -----------------------------------------------------------------------------
macro(SWIGXML_SOURCE LANG GENDIR ARGS OUT DEPS)
    swig_source_absolute_src(ABS_DEPS ${${DEPS}})
    get_target_property(SWIGXML SwigXML LOCATION)

    swig_source_xml(XML_OUT ${DEPS} ${ARGN})
    foreach (ARG ${XML_OUT})


        get_filename_component(IFILE ${ARG} ABSOLUTE)
        string(REPLACE ".xml" "${LANG}" OFILE ${IFILE})

        get_filename_component(INAME ${IFILE} NAME)
        get_filename_component(ONAME ${OFILE} NAME)


        add_custom_command(
            OUTPUT  ${OFILE}
            COMMAND ${SWIGXML} ${OFILE} ${IFILE}
            DEPENDS ${SWIGXML} ${IFILE} ${ABS_DEPS} ${LIB_FILES}
            WORKING_DIRECTORY ${SWIG_LIB_ROOT}
            COMMENT "SwigXML ==> ${ONAME}"
        )
        list(APPEND ${OUT} ${OFILE})
        list(APPEND ${OUT} ${ARG})
    endforeach(ARG)
endmacro(SWIGXML_SOURCE)
# -----------------------------------------------------------------------------
#
# External XML Tests
#
# -----------------------------------------------------------------------------
macro(SWIGXML_SOURCE_XML OUT DEPS)
    swigxml_source(".cpp" "Generated" ARGS ${OUT} ${DEPS} ${ARGN})
endmacro(SWIGXML_SOURCE_XML)
