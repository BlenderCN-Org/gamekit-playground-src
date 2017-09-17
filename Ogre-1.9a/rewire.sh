#!/bin/bash
echo "Rewire ogre to ogre-pendencies! This assumes that ogre-pendencies is in the same directory structure as gamekit"
echo "Folder: $PWD"
rm RenderSystems
rm CMake
rm Components
rm OgreMain
rm PlugIns
ln -s $PWD/../../gamekit-pendencies/ogre-git/RenderSystems
ln -s $PWD/../../gamekit-pendencies/ogre-git/CMake
ln -s $PWD/../../gamekit-pendencies/ogre-git/Components
ln -s $PWD/../../gamekit-pendencies/ogre-git/OgreMain
ln -s $PWD/../../gamekit-pendencies/ogre-git/PlugIns
