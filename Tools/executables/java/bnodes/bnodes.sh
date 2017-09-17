#!/bin/bash

echo generate bnodes for file $1

BASEDIR=$(dirname $0)


/usr/bin/java -jar $BASEDIR/bnodes.jar $1 $2 $3 $4
