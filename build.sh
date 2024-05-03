#/!bin/bash

SOURCE_DIR=$(pwd)
BUILD_DIR="build"
if [ ! -d $BUILD_DIR ]; then
	mkdir $BUILD_DIR
fi

cd $BUILD_DIR|| exit

cmake ..
make

# run tests
./UT/ut

cd $SOURCE_DIR
