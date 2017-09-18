This is the source-code to build gamekit-playground ( https://github.com/dertom95/gamekit-playground )

*caution* 
 
This is some experimental gamekit-fork I started January 2013. So be aware that there are lots of deadends still in the code. Actually very ugly. Ok,...use at your own risk. 
 
This repository works under Linux and afaik not under Win and MacOSX. With my scripts you can create automatically create the gamekit-playground which can build runtimes for linux/win/android/bb10/playbook/ webgl(compiling but actually not working anymore) 
 
Feel free to make it win or macosx conform again. 

how to install.
0) Add environment-paths. e.g. to /etc/environment. Not sure where exaclty I do you those. But at least the GAMEKIT one, is really used ;)
GAMEKIT="/home/ttrocha/_dev/extprojects/gamekit"
GAMEKIT_BUILD="/home/ttrocha/_dev/extprojects/_build/gamekit"

1) clone
git clone https://github.com/dertom95/gamekit-playground-src.git

2) download deps. This will create a folder along the current one with name gamekit-pendencies
cd gamekit-playground-src
./_clone_deps.sh
3) Install SDKs and set environment-variables. e.g.
export MXE_PATH="/opt/mxe"
export BB_PLAYBOOK_NDK="/home/ttrocha/_dev/_lib/blackberry/bbndk-2.1.0"
export #BB_PLAYBOOK_NDK=""
export BB_BB10_NDK="/home/ttrocha/_dev/_lib/blackberry/bbndk"
export EMSCRIPTEN_SDK="/home/ttrocha/_dev/_lib/emsdk_portable"
export ANDROID_NDK="/home/ttrocha/_dev/_lib/android/android-ndk-r8d"

cautions with ANDROID_NDK the toolchain is at least compatible with ndk-r8d and you need build-tools(23.0.3) and targets(20,23) installed. There you might need to fiddle around a bit. In the Samples/AndroidDemo/Demo-Folder you need to make sure the ndk is found. Had problems there lately

4) build playground: first parameter: build-folder, where all runtimes are build in
                     second parameter: where the playground folder is located afterwards (needs to be a absolute path)
./_build_playground.sh ../build $PWD../playground

5) go to you playground and build. For more instructions here (https://github.com/dertom95/gamekit-playground)
cd ../playground
./package_all_linux.sh
