device=$1 
password=$2

echo "sourcing hardcoded blackberry toolchain!!! If that doesnt work, modify the path!"
source /home/ttrocha/_dev/_lib/blackberry/bbndk/bbndk-env_10_3_1_995.sh

echo "package => gk.bar"

blackberry-nativepackager -package gk.bar -devMode bar-descriptor.xml

echo "install to device"

#blackberry-nativepackager -installApp -device $1 -password $2 gk.bar

echo blackberry-nativepackager -installApp -device $device -password $password gk.bar

blackberry-nativepackager -installApp -device $device -password $password gk.bar

