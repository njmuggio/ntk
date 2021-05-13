cmake_minimum_required(VERSION 3.14)
project(ntk VERSION 0.1.2 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(ntk ntk.c ntk.h)
target_include_directories(ntk PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(ntk PROPERTIES C_CLANG_TIDY "clang-tidy")
set_target_properties(ntk PROPERTIES C_CPPCHECK "cppcheck;--std=c99;--enable=warning,style,performance,portability")

add_subdirectory(test)
