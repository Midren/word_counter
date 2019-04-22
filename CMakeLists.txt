cmake_minimum_required(VERSION 3.14)
project(parallel_reading)

set(CMAKE_CXX_STANDARD 14)
find_package(LibArchive REQUIRED)
find_package(Boost REQUIRED COMPONENTS locale filesystem)

add_executable(parallel_reading main.cpp zip.cpp)
target_link_libraries(parallel_reading
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
        )
add_executable(single_thread single_thread.cpp)

target_link_libraries(single_thread LINK_PUBLIC ${Boost_LIBRARIES})