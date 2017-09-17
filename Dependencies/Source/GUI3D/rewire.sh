#!/bin/bash
echo "Rewire gui3d-pendencies! This assumes that ogre-pendencies is in the same directory structure as gamekit"
echo "Folder: $PWD"
rm source 
ln -s ../../../../gamekit-pendencies/gui3d/build/source
