#! /bin/bash

source ../../version

BUILD_DIR=$PWD
#./build-simavr-$_ARCH

cd $BUILD_DIR

$_CROSS$QMAKE_EXEC

cd build_$_ARCH$_BITS

make
