#!/usr/bin/env bash

if ! type g++ >/dev/null 2>&1; then
    echo '正在安装g++'
    sudo apt install g++
fi

if ! type cmake >/dev/null 2>&1; then
    echo '正在安装cmake'
    sudo apt install cmake
fi

if ! type make >/dev/null 2>&1; then
    echo '正在安装make'
    sudo apt install make
fi

if ! type openssl >/dev/null 2>&1; then
    echo '正在安装openssl'
    sudo apt install openssl
fi

if ! type java >/dev/null 2>&1; then
    echo '正在安装java'
    sudo apt install openjdk-8-jdk
fi

if ! type mvn >/dev/null 2>&1; then
    echo '正在安装maven'
    sudo apt install maven
fi

if ! type mongo >/dev/null 2>&1; then
    echo '正在安装MongoDB'
    sudo apt install mongodb
fi

if [ -d ./build ];then 
    echo '删除build'
    rm -rf build/
fi

echo "创建目录: build"
mkdir build
cd build/
cmake ../
echo "do make"
make
cd ../


echo '生成jar包'
rm backup-system-web-0.0.1-SNAPSHOT.jar
mvn clean
mvn package
mv target/backup-system-web-0.0.1-SNAPSHOT.jar ./
