#!/bin/bash

# Android working with ndk-8d:
#   linux64: http://dl.google.com/android/ndk/android-ndk-r8d-linux-x86_64.tar.bz2
#   linux32: http://dl.google.com/android/ndk/android-ndk-r8d-linux-x86.tar.bz2
#   windows: http://dl.google.com/android/ndk/android-ndk-r8d-windows.zip
#   osx32  : http://dl.google.com/android/ndk/android-ndk-r8d-darwin-x86.tar.bz2 
#   osx64  : http://dl.google.com/android/ndk/android-ndk-r8d-darwin-x86_64.tar.bz2
#   For other old version, here is a description: http://stackoverflow.com/questions/6849981/where-do-i-find-old-versions-of-android-ndk

#### CONFIG #####
MAKE_OPTION="-j8"
MXE_PATH="/opt/mxe"
BB_PLAYBOOK_NDK="/home/ttrocha/_dev/_lib/blackberry/bbndk-2.1.0"
#BB_PLAYBOOK_NDK=""
BB_BB10_NDK="/home/ttrocha/_dev/_lib/blackberry/bbndk"
EMSCRIPTEN_SDK="/home/ttrocha/_dev/_lib/emsdk_portable"
ANDROID_NDK="/home/ttrocha/_dev/_lib/android/android-ndk-r8d"
################

if [[ $# -ne 2 ]]; then
  echo "Usage:./_build_playground.sh [build-dir] [absolute(!) playground-dir] "
  exit 0;
fi
echo "First $1 Second $2"
START_DIR=$PWD
GAMEKIT_DIR=$( cd $(dirname $0) ; pwd )
BUILD_DIR=$1
PLAYGROUND_DIR=$2

mkdir $BUILD_DIR

cd $BUILD_DIR||{
  echo "UNKNOWN BUILD-DIR:$BUILD_DIR"
}
BUILD_DIR=$PWD

echo building runtimes
echo build-dir:$BUILD_DIR playground-dir:$PLAYGROUND_DIR
echo gamekit-dir:$GAMEKIT_DIR

echo "check playground"
if [[ ! -d $PLAYGROUND_DIR ]]; then
  mkdir -p "$PLAYGROUND_DIR"
  echo "Couldn't find playground at $PLAYGROUND_DIR! Copy the template playground!"
  cp -r $GAMEKIT_DIR/SDK/gamekit-playground-template/* $PLAYGROUND_DIR
fi



# $1=os $2=additional cmake-switches
function cmake_and_compile {
	COMPILE_OS=$1
	CMAKE_SWITCHES=$2 
        CMAKE_DIR="gamekit-"$COMPILE_OS
	echo "cmake: $COMPILE_OS into $CMAKE_DIR"
	cd $BUILD_DIR

	if [[ ! -d $CMAKE_DIR ]]; then
	  mkdir $CMAKE_DIR
	fi

	cd $CMAKE_DIR
	cmake -DCMAKE_BUILD_TYPE=Release -DPLAYGROUND_PATH=$PLAYGROUND_DIR $GAMEKIT_DIR  $CMAKE_SWITCHES || {
	  echo "Something went wrong with CMAKE for $COMPILE_OS"
	  cmake-gui .
	  exit 1;
	}

	make $MAKE_OPTION || {
	  echo "Something went wrong in the compile process for OS:$COMPILE_OS"
	  echo "Compile-Folder $PWD";
	  exit 1;
	}

	cd ..
}

# build linux
cmake_and_compile linux "-DOGREKIT_UPDATE_LUA_DOCS=ON" 
# create docs

cd gamekit-linux/Docs/LuaAPI
doxygen Doxyfile.txt
rm -Rf $PLAYGROUND_DIR/docs/lua-api
mv html $PLAYGROUND_DIR/docs/lua-api
cd $BUILD_DIR.

# build windows with mxe and it's mingw-toolchain
if [[ $MXE_PATH != "" ]]; then
  MXE_TOOLCHAIN=$MXE_PATH"/usr/i686-w64-mingw32.static/share/cmake/mxe-conf.cmake"
  cmake_and_compile windows -DCMAKE_TOOLCHAIN_FILE=$MXE_TOOLCHAIN
fi

# build bb-playbook 
if [[ $BB_PLAYBOOK_NDK != "" ]]; then
  BB_PB_ENV_FILE=$BB_PLAYBOOK_NDK"/bbndk-env.sh"
  source $BB_PB_ENV_FILE
  BB_PB_TOOLCHAIN=$GAMEKIT_DIR"/CMake/toolchain/playbook.toolchain.cmake"
  cmake_and_compile playbook -DCMAKE_TOOLCHAIN_FILE=$BB_PB_TOOLCHAIN
fi

# build bb-b10 
if [[ $BB_BB10_NDK != "" ]]; then
  BB_BB10_ENV_FILE=$BB_BB10_NDK"/bbndk-env_10_3_1_995.sh"
  source $BB_BB10_ENV_FILE
  BB_BB10_TOOLCHAIN=$GAMEKIT_DIR"/CMake/toolchain/bb10.toolchain.arm.cmake"
  cmake_and_compile bb10 -DCMAKE_TOOLCHAIN_FILE=$BB_BB10_TOOLCHAIN
fi

# android
if [[ $ANDROID_NDK != "" ]]; then
  ANDROID_TOOLCHAIN=$GAMEKIT_DIR"/CMake/toolchain/android.toolchain.cmake"
  cmake_and_compile android -DCMAKE_TOOLCHAIN_FILE=$ANDROID_TOOLCHAIN
fi

# emscripten / web
if [[ $EMSCRIPTEN_SDK != "" ]]; then
  EM_ENV_FILE=$EMSCRIPTEN_SDK"/emsdk_env.sh"
  source $EM_ENV_FILE
  EM_TOOLCHAIN=$EMSCRIPTEN"/cmake/Modules/Platform/Emscripten.cmake"
  cmake_and_compile em -DCMAKE_TOOLCHAIN_FILE=$EM_TOOLCHAIN
fi


#############
# Create-Plugin with current nodes-configuration
#############

cd $GAMEKIT_DIR/BlenderAddon
rm BlenderAddon-Experimental.zip
zip -r BlenderAddon-Experimental.zip *
echo cp BlenderAddon-Experimental.zip $PLAYGROUND_DIR/blender-addon/blender-addon-experimental.zip
cp BlenderAddon-Experimental.zip $PLAYGROUND_DIR/blender-addon/blender-addon-experimental.zip



