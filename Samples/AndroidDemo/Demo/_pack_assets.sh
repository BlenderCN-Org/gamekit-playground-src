#!/bin/bash

cd assets

shopt -s globstar
if [ -e "t" ]; then
	rm -Rf t
fi

mkdir t

cp **/*.png t 
cp **/*.jpg t
cp **/*.tga t
cp **/*.gorilla t
cp **/*.atlas t

cd t
zip assets.zip *
cd ..

if [ -e "t/assets.zip" ]; then
	echo "updated asset file"
	cp t/assets.zip .
else
	echo "no asset file generated???"
fi

if [ -e "t" ]; then
	rm -Rf t
fi

cd ..
