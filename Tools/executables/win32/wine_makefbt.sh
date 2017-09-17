#!/bin/bash

# caution: is file is copied at compile time when crosscompiling with mingw on linux

echo MY FOLDER: $PWD

/usr/bin/wine ../../Compiler/makefbt.exe $1 $2 $3 $4 $5
