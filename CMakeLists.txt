cmake_minimum_required(VERSION 3.12)
project(parallel_reading)

set(CMAKE_CXX_STANDARD 14)
find_package(LibArchive REQUIRED)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREAD ON)
find_package(Boost REQUIRED COMPONENTS locale filesystem system)

add_executable(parallel_reading main.cpp zip.cpp)
add_executable(single_thread single_thread.cpp utils.cpp)
add_executable(multiple_threads multiple_threads.cpp utils.cpp)

target_link_libraries(parallel_reading LINK_PUBLIC ${LibArchive_LIBRARIES} ${Boost_LIBRARIES})
target_link_libraries(single_thread LINK_PUBLIC ${Boost_LIBRARIES})
target_link_libraries(multiple_threads LINK_PUBLIC ${Boost_LIBRARIES})
