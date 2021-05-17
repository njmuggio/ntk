cmake_minimum_required(VERSION 3.14)
project(ntk VERSION 0.1.2 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(ntk ntk.c ntk.h)
target_include_directories(ntk PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

find_program(CLANG_TIDY_BIN NAMES clang-tidy clang-tidy-11 clang-tidy-10 clang-tidy-9)
if (CLANG_TIDY_BIN)
  message(STATUS "Found clang-tidy: ${CLANG_TIDY_BIN}")
  set_target_properties(ntk PROPERTIES C_CLANG_TIDY "${CLANG_TIDY_BIN}")
else()
  message(STATUS "No clang-tidy found, skipping checks")
endif()

find_program(CPPCHECK_BIN cppcheck)
if (CPPCHECK_BIN)
  message(STATUS "Found cppcheck: ${CPPCHECK_BIN}")
  set_target_properties(ntk PROPERTIES C_CPPCHECK "cppcheck;--std=c99;--enable=warning,style,performance,portability")
else()
  message(STATUS "No cppcheck found, skipping checks")
endif()

add_subdirectory(test)
