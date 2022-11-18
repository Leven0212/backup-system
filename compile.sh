#!/usr/bin/env bash
rm -rf build/
echo "创建目录: build"
mkdir build
cd build/
cmake ../
echo "do make"
make

cd ../