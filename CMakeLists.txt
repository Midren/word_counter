cmake_minimum_required(VERSION 3.12)
project(parallel_reading)

set(CMAKE_CXX_STANDARD 14)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREAD ON)
set(CMAKE_CXX_COMPILER g++)

find_package(LibArchive REQUIRED)
find_package(Boost REQUIRED COMPONENTS locale filesystem system)
find_package(Threads REQUIRED)

add_executable(single_thread single_thread.cpp config_parser.cpp zip.cpp utils.cpp)
add_executable(multiple_threads multiple_threads.cpp utils.cpp zip.cpp config_parser.cpp)

target_link_libraries(single_thread LINK_PUBLIC ${Boost_LIBRARIES} ${LibArchive_LIBRARIES}  )

target_link_libraries(multiple_threads LINK_PUBLIC ${Boost_LIBRARIES} ${LibArchive_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
