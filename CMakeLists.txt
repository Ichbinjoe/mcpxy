cmake_minimum_required(VERSION 3.12)
project(mcpxy)

set(CMAKE_CXX_STANDARD 17)

configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
        ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
        EXCLUDE_FROM_ALL)

add_subdirectory(proto)

add_executable(mcpxy main.cpp)
target_link_libraries(mcpxy PRIVATE proto)
target_include_directories(mcpxy PUBLIC ${CMAKE_SOURCE_DIR})


add_executable(mcpxy-test test.cpp)
target_link_libraries(mcpxy-test gmock_main)
target_include_directories(mcpxy-test PUBLIC ${CMAKE_SOURCE_DIR})