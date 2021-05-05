cmake_minimum_required(VERSION 3.14)
project(ntk VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_library(ntk ntk.c ntk.h)
target_include_directories(ntk PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(test)
