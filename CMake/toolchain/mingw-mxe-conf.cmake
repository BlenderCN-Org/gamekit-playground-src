set(CMAKE_SYSTEM_NAME Windows)
set(MSYS 1)
set(BUILD_SHARED_LIBS OFF)
set(LIBTYPE STATIC)
set(CMAKE_PREFIX_PATH /opt/mxe/usr/i686-w64-mingw32.static)
set(CMAKE_FIND_ROOT_PATH /opt/mxe/usr/i686-w64-mingw32.static)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_C_COMPILER /opt/mxe/usr/bin/i686-w64-mingw32.static-gcc)
set(CMAKE_CXX_COMPILER /opt/mxe/usr/bin/i686-w64-mingw32.static-g++)
set(CMAKE_Fortran_COMPILER /opt/mxe/usr/bin/i686-w64-mingw32.static-gfortran)
set(CMAKE_RC_COMPILER /opt/mxe/usr/bin/i686-w64-mingw32.static-windres)
set(CMAKE_MODULE_PATH "/opt/mxe/usr/share/cmake/modules" ${CMAKE_MODULE_PATH}) # For mxe FindPackage scripts
set(CMAKE_INSTALL_PREFIX /opt/mxe/usr/i686-w64-mingw32.static CACHE PATH "Installation Prefix")
set(CMAKE_BUILD_TYPE Release CACHE STRING "Debug|Release|RelWithDebInfo|MinSizeRel")
set(CMAKE_CROSS_COMPILING ON) # Workaround for http://www.cmake.org/Bug/view.php?id=14075
set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> -O coff <FLAGS> <DEFINES> -o <OBJECT> <SOURCE>") # Workaround for buggy windres rules

file(GLOB mxe_cmake_files
    "/opt/mxe/usr/i686-w64-mingw32.static/share/cmake/mxe-conf.d/*.cmake"
)
foreach(mxe_cmake_file ${mxe_cmake_files})
    include(${mxe_cmake_file})
endforeach()
