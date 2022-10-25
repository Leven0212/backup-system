

find_package(OpenSSL REQUIRED)
include_directories(include)

add_executable(backup main.cc error.cc basefunction.cc AES.cpp)

find_package(OpenSSL REQUIRED)
target_link_libraries(backup OpenSSL::SSL)