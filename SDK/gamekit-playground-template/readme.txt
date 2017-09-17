gamekit-playground

what is this?
-------------

gamekit playground is actually gamekit ( https://github.com/gamekit-developers/gamekit ) 
a game engine which is based on ogre3d, bullet and other opensource-libs. 

Only that I tried to include new stuff in a very "just do it"-way which is ended up in a very fragile version.
Most things actually do work but the integration was not always intended for someone else to see it...
Especially the experimental blender-addon will crash blender quite often (I did really nasty things there. The new
plugin as its cool features, but I will need to rewrite it from ground up)

Nonetheless I now do want to opensource my "gamekit playground" (therefore the name). But please have in mind, that 
if you want a working stressless engine, stop here and go with unity or unreal....

This file is part of the gamekit-playground-deployment-package. At least using linux as development platform 
you should be able to deploy your gamekit-app to Linux,Windows,Android,Blackberry BB10,Blackberry Playbook and
Web [experimental but basics seem to work](html/js with emscripten).


How to install
--------------

1)Startup blender: File=>User Preferences=>Add-Ons=>Install from File=>[navigate to the playground-folder's blender-addon] and choose blenderaddon.zip or the experimental version.
2)Select gamekit-renderer. On top where you see "Blender Render" choose "Gamekit".
3)On the Render-Panel under "Gamekit Runtime" choose the runtime file-icon and navigate to the playground=>runtimes=>your os=>AppOgreKit[.exe]
(Sorry no support for macosx at the moment! I did try a complete night and day to get it to work. Without success. You can use wine on the windows-version though. Installing wine via brew did work for me)
4)Save your blend before starting the engine. Otherwise it will not work...

Auto-Packaging of all supported Platforms(win,lin,android,bb10,playbook,web)

What you need?
--------------
First of all you need to create your gamekit-project inside the playground-project folder and name you master-blend project.blend


Deploying:

Linux and Windows should work out of the box. 
linux: just starting "./package_all_linux.sh [gamename]" will produce a folder named _package
where you will find zips with the game-pack.
windows: starting  "./package_win.bat" will call the linux version with the help of a windows-bash (no zips are created)

In order for the other platforms to work, you need to install their SDKs:

For web:
Install emscripten-sdk: https://kripken.github.io/emscripten-site/docs/getting_started/downloads.html
And modify the package_all_linux.sh script to point to the emsdk-portable-root.

For bb-playbook: https://developer.blackberry.com/playbook/native/download/
For bb-bb10: https://developer.blackberry.com/native/downloads/
Also modify the package_all_linux.sh to point to the respective SDKs

For android:
Modify the runtimes/android/local.properties to point to your SDK and NDK (not sure if you need the NDK at this point)


-----------------------------------

I know that instruction is very confuse but it is a first version.

There are some docs for the lua-api in [playground]/docs/lua-api (search the index.html). Not optimal but better than nothing.

For Questions: Please refer to the forum at http://gamekit.org