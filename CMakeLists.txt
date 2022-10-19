cmake_minimum_required(VERSION 3.16)
project(BackupProject)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(code)

SET(CMAKE_BUILD_TYPE "Debug")  
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")  
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall") 