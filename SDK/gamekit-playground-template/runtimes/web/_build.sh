cd ../../project
python $EMSCRIPTEN/tools/file_packager.py ../runtimes/web/project.data --js-output=../runtimes/web/loader.js --preload . --exclude *
cd ../runtimes/web
