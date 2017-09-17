echo "Upload gk.bar to simualtor!"

source /home/ttrocha/_dev/_libs/blackberry/bb10/bbndk-env.sh

#make -j2

#echo "compile without -rdynamic (silly but now good other way)"

#cd Samples/BlackBerryDemo/Shared
#/home/ttrocha/_dev/_libs/blackberry/bbndk2.1/host/linux/x86/usr/bin/qcc    -V4.4.2,gcc_ntoarmv7le -Y_gpp -D__QNX__  -V4.4.2,gcc_ntoarmv7le -Y_gpp -D__QNX__ -fexceptions -marm -Os -finline-limit=64  -lstdc++ -lm -lEGL -lGLESv2 -lbps -lscreen CMakeFiles/bbkit.dir/main.cpp.o CMakeFiles/bbkit.dir/gkAndroidGame.cpp.o  -o bbkit -L/home/ttrocha/_dev/_libs/blackberry/bbndk2.1/target/qnx6/armle-v7/usr/lib -L/home/ttrocha/_dev/extprojects/gamekit-dev/Dependencies/Source/../blackberry/libcurl-7.29.0/arm ../../../Engine/libOgreKitCore.a ../../../Bin/lib/libOgreMainStatic.a ../../../Bin/lib/libOgreOverlayStatic.a ../../../Dependencies/Source/FreeImage/libFreeImage.a ../../../Dependencies/Source/FreeType/libfreetype.a ../../../Dependencies/Source/GameKit/Utils/libGameKitUtils.a ../../../Dependencies/Source/OIS/libOIS.a ../../../Dependencies/Source/FreeImage/ZLib/libZLib.a ../../../Dependencies/Source/GameKit/AnimKit/libAnimKit.a ../../../Dependencies/Source/CppTweener/libcpptweener.a -lcurl ../../../Dependencies/Source/GUI3D/libGUI3D.a ../../../FileTools/File/libfbtFile.a ../../../FileTools/FileFormats/Blend/libbfBlend.a ../../../Bin/lib/libOgreRTShaderSystemStatic.a ../../../Dependencies/Source/Codecs/libOggVorbis.a ../../../Dependencies/Source/Lua/lua/libLua.a ../../../Dependencies/Source/ZZipLib/libZZipLib.a -lGLESv2 -lbps -lscreen -lEGL -lm -lOpenAL ../../../Dependencies/Source/FreeImage/ZLib/libZLib.a ../../../bullet/src/BulletDynamics/libBulletDynamics.a ../../../bullet/src/BulletCollision/libBulletCollision.a ../../../bullet/src/LinearMath/libLinearMath.a ../../../Bin/lib/libPlugin_ParticleFXStatic.a ../../../Bin/lib/libRenderSystem_GLES2Static.a ../../../Bin/lib/libOgreMainStatic.a ../../../Dependencies/Source/libRocket/Build/libRocketControls_d.a ../../../Dependencies/Source/libRocket/Build/libRocketDebugger_d.a ../../../Dependencies/Source/libRocket/Build/libRocketCore_d.a -Wl,-rpath,/home/ttrocha/_dev/_libs/blackberry/bbndk2.1/target/qnx6/armle-v7/usr/lib:/home/ttrocha/_dev/extprojects/gamekit-dev/Dependencies/Source/../blackberry/libcurl-7.29.0/arm

#/home/ttrocha/_dev/_libs/blackberry/bbndk2.1/host/linux/x86/usr/bin/qcc -V4.4.2,gcc_ntoarmv7le -Y_gpp -D__QNX__ -V4.4.2,gcc_ntoarmv7le -Y_gpp -D__QNX__ -fexceptions -marm -Os -finline-limit=64 -lstdc++ -lm -lEGL -lGLESv2 -lbps -lscreen CMakeFiles/bbkit.dir/main.cpp.o CMakeFiles/bbkit.dir/gkAndroidGame.cpp.o -o bbkit -L$QNX_TARGET/armle-v7/usr/lib ../../../Engine/libOgreKitCore.a ../../../Bin/lib/libOgreMainStatic.a ../../../Bin/lib/libOgreOverlayStatic.a ../../../Dependencies/Source/FreeImage/libFreeImage.a ../../../Dependencies/Source/FreeType/libfreetype.a ../../../Dependencies/Source/GameKit/Utils/libGameKitUtils.a ../../../Dependencies/Source/OIS/libOIS.a ../../../Dependencies/Source/FreeImage/ZLib/libZLib.a ../../../Dependencies/Source/GameKit/AnimKit/libAnimKit.a ../../../Dependencies/Source/CppTweener/libcpptweener.a ../../../Dependencies/Source/GUI3D/libGUI3D.a ../../../FileTools/File/libfbtFile.a ../../../FileTools/FileFormats/Blend/libbfBlend.a ../../../Bin/lib/libOgreRTShaderSystemStatic.a ../../../Dependencies/Source/Codecs/libOggVorbis.a ../../../Dependencies/Source/Lua/lua/libLua.a ../../../Dependencies/Source/ZZipLib/libZZipLib.a -lGLESv2 -lbps -lscreen -lEGL -lm -lOpenAL ../../../Dependencies/Source/FreeImage/ZLib/libZLib.a ../../../bullet/src/BulletDynamics/libBulletDynamics.a ../../../bullet/src/BulletCollision/libBulletCollision.a ../../../bullet/src/LinearMath/libLinearMath.a ../../../Bin/lib/libPlugin_ParticleFXStatic.a ../../../Bin/lib/libRenderSystem_GLES2Static.a ../../../Bin/lib/libOgreMainStatic.a ../../../Dependencies/Source/libRocket/Build/libRocketControls_d.a ../../../Dependencies/Source/libRocket/Build/libRocketDebugger_d.a ../../../Dependencies/Source/libRocket/Build/libRocketCore_d.a -Wl,-rpath,/home/ttrocha/_dev/_libs/blackberry/bbndk2.1/target/qnx6/armle-v7/usr/lib
#cd ../../..

echo "package => gk.bar"

blackberry-nativepackager -package gk.bar -devMode bar-descriptor.xml

echo "install "
echo $1
echo $2

blackberry-nativepackager -installApp -device $1  gk.bar

