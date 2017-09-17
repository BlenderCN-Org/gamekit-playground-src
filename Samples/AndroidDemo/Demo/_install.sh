#./_pack_assets.sh
#adb uninstall org.gamekit.jni
adb uninstall org.kijato.raupe
ant debug
adb install bin/OgreKit-debug.apk
