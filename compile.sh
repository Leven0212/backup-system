#!/usr/bin/env bash

if ! type openssl >/dev/null 2>&1; then
    sudo apt-get install openssl;
fi
if ! type cmake >/dev/null 2>&1; then
    sudo apt install cmake;
fi

rm -rf build/
echo "创建目录: build"
mkdir build
cd build/
cmake ../
echo "do make"
make
cd ../


if ! type mvn >/dev/null 2>&1; then
    echo 'maven 未安装';
else
    echo 'maven 已安装';
fi