cmake_minimum_required(VERSION 3.29)
project(mr-stl-tests)

include(CPM.cmake)
CPMAddPackage("gh:google/googletest@1.14.0")
CPMAddPackage("gh:google/benchmark@1.5.2")

set(CMAKE_CXX_STANDARD 23)

add_executable(${CMAKE_PROJECT_NAME} tests.cpp)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ..)
target_link_libraries(${CMAKE_PROJECT_NAME} gtest gtest_main)
target_link_libraries(${CMAKE_PROJECT_NAME} benchmark)
