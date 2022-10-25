

find_package(OpenSSL REQUIRED)
include_directories(include)

add_executable(backup main.cc error.cc basefunction.cc AES.cpp)

find_package(OpenSSL REQUIRED)
target_link_libraries(backup OpenSSL::SSL)

add_library(_backup SHARED  main.cc error.cc basefunction.cc AES.cpp)
target_link_libraries(_backup OpenSSL::SSL)