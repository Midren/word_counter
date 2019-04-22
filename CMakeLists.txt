cmake_minimum_required(VERSION 3.14)
project(parallel_reading)

set(CMAKE_CXX_STANDARD 14)
find_package(LibArchive REQUIRED)
find_package(BOOST REQUIRED)
add_executable(parallel_reading main.cpp zip.cpp zip.h)