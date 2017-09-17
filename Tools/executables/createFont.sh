# call: createFont.sh outputFolder outputName
# createFont.sh out myfont
$GAMEKIT/Tools/executables/win32/bmfontgen.exe -bmsize 1000x1000 -fontdialog -output $1/$2
#convert $1/$2-0.png -gravity West -background black -splice 0x5 
#		    -gravity North -background black -splice 0x5 -trim +repage $1/$2-0.png
