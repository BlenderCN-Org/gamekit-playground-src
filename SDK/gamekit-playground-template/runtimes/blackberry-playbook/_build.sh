echo "sourcing hardcoded blackberry toolchain!!! If that doesnt work, modify the path!"
echo "source /home/ttrocha/_dev/_libs/blackberry/bbndk2.1/bbndk-env.sh"
source /home/ttrocha/_dev/_lib/blackberry/bbndk-2.1.0/bbndk-env.sh

echo "package => gk.bar"

blackberry-nativepackager -package gk.bar -devMode bar-descriptor.xml

echo "install to device"

blackberry-nativepackager -installApp -device $1 -password $2 gk.bar

