cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 17)

find_library(UUID_LIB uuid)

add_library(proto Types.cpp)
target_link_libraries(proto PUBLIC "${UUID_LIB}")
