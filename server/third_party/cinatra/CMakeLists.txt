cmake_minimum_required(VERSION 3.15)
project(cinatra)

include_directories(include)

#the thread library of the system.
find_package(Threads REQUIRED)

include(cmake/build.cmake)
include(cmake/develop.cmake)

add_library(cinatra INTERFACE)

target_include_directories(cinatra
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)