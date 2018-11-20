cmake_minimum_required(VERSION 3.12)
project(mcpxy)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(proto)

add_executable(mcpxy main.cpp)

target_link_libraries(mcpxy PRIVATE proto)