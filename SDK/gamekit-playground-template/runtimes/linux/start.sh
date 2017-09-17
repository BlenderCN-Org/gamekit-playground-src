#!/bin/bash

BASEDIR=$(dirname $0)
echo $BASEDIR

$BASEDIR/AppOgreKit $1  $2  $3  $4  $5  $6  $7  $8  $9 & 
echo $1 $2 $3  $4  $5  $6  $7  $8  $9 > $BASEDIR/last_call
