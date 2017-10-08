#!/bin/bash

GAMEKIT_DIR=$( cd $(dirname $0) ; pwd )

echo "creating bnodes..."
cd $GAMEKIT_DIR/BNodes
./update_bnodes.sh
echo "...finished!"

cd $GAMEKIT_DIR/BlenderAddon
rm BlenderAddon-Experimental.zip
echo "zipping..."
zip -r BlenderAddon-Experimental.zip * > zip.log
echo "...finished"
cd ..


