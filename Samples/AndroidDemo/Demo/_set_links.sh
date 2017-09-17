if [ -z $1 ]; then
	echo "Android-Path-Setter"
	echo "Usage: _set_paths.sh main_dir relpath_gameblend relpath_mediafolder relpath_luafolder"
else
	echo "Set paths"
	echo "main-path: $1"
	echo "media: $1/$3"
	echo "gk_android.blend: $1/$2"
	echo "res-cfg: $1/res.cfg"
	echo "lua: $1/$4"

	cd assets

	rm media
	if [ ! -z "$3" ]; then
		if [ -d "$1/$3" ]; then
			echo "media: $1/$3"
			ln -s $1/$3
		fi	
	fi
	
	rm gk_android.blend
	if [ ! -z "$2" ]; then
		if [ -e "$1/$2" ]; then
			echo "gk_android.blend: $1/$2"
			ln -s $1/$2 gk_android.blend
		fi
	fi

	rm res.cfg
	if [ -e "$1/res.cfg" ]; then
		echo "res-cfg: $1/res.cfg"
		ln -s $1/res.cfg 
	else
		echo "No res-cfg at: $1/res.cfg"
	fi

	rm lua
	if [ ! -z "$4" ]; then
		echo "lua: $1/$4"
		if [ -e "$1/$4" ]; then
			ln -s $1/$4 lua
		fi
	fi
fi
