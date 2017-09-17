OLD_DIR=$PWD

if [ -d "../gamekit-pendencies" ]; then
	echo "directory exists [ok]"
else
	echo "create directory ../gamekit-pendencies"
	mkdir ../gamekit-pendencies
fi

cd ../gamekit-pendencies

echo "Cloning dependencies"

if [ ! -d "recastnavigation" ]; then
	echo "Recastnavigation"
	git clone https://github.com/memononen/recastnavigation.git
fi

if [ ! -d "ogre-git" ]; then
	echo "Ogre-Git"
	git clone https://dertom@bitbucket.org/dertom/ogre-git.git
	cd ogre-git
	# set the connection to the original-repository
	git remote add origin-repo hg::http://bitbucket.org/sinbad/ogre/
	# switch to gk-branch
	git checkout gk
	cd ..
fi



if [ ! -d "thrift" ]; then
	echo "Thrift"
	git clone https://dertom@bitbucket.org/dertom/thrift.git
	cd thrift
	# set connection to the original-repository
	git remote add origin-repo https://git-wip-us.apache.org/repos/asf/thrift.git
	# switch to branch gk
	git checkout gk
	cd ..
fi

if [ ! -d "gui3d" ]; then
        echo "gui3d"
        git clone https://dertom@bitbucket.org/dertom/gui3d.git
        cd gui3d
        # set connection to the original-repository
        git remote add origin-repo https://github.com/Valentin33/Gui3D.git
        git checkout gk
        cd ..
fi

if [ ! -d "lessc" ]; then
        echo "lessc"
        git clone https://dertom@bitbucket.org/dertom/lessc.git
        cd gui3d
        # set connection to the original-repository
        git remote add origin-repo https://github.com/BramvdKroef/clessc.git 
        git checkout gk
        cd ..
fi

cd $OLD_DIR
./_rewire.sh