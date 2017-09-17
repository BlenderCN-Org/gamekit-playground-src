#!/bin/bash
java -cp $GAMEKIT/Tools/executables/java/libgdx-texturepacker/gdx.jar:$GAMEKIT/Tools/executables/java/libgdx-texturepacker/gdx-tools.jar com.badlogic.gdx.tools.imagepacker.TexturePacker2 $1 $2
