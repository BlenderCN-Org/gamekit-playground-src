if [ $1 ] 
then
	if [ ! -f $1/lib/libprotobuf.a ]
	then
		if [ ! -f protobuf-2.5.0.tar.gz ] 
		then
			wget http://protobuf.googlecode.com/files/protobuf-2.5.0.tar.gz 
			tar xvf protobuf-2.5.0.tar.gz
			mv protobuf-2.5.0 protobuf
			cd protobuf
			./configure --prefix=$1
		else
			cd protobuf
		fi
		make
		make install 
		cd ..
	fi
else
	echo "ERROR: you need to provide an installation path"
fi
