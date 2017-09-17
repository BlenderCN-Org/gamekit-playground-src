OLD_DIR=$PWD

echo "Rewire Ogre"
cd Ogre-1.9a
./rewire.sh

echo "Rewire Recast"
cd $OLD_DIR/Dependencies/Source/RecastDetour
./rewire.sh

cd $OLD_DIR/BNodes
echo "Generate BNode-Code"
./update_bnodes.sh > bnodes.log


cd $OLD_DIR
