cmake_minimum_required(VERSION 3.9)

# Availables options
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INSTALL_PREFIX /usr/lib/slurm)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_STATIC_LIBRARY_PREFIX "")

project(
  spank_report
  VERSION 1.0.0
  DESCRIPTION "SPANK reporting plugin")

add_compile_options(
  -O3
  -Wno-unused-parameter
  -m64
  -std=c99
  -pedantic
  -Wall
  -Wextra
  -Wshadow
  -Wdouble-promotion
  -Wformat=2
  -Wpointer-arith
  -Winit-self
  -Wcast-align
  -Wcast-qual
  -Wunreachable-code
  -Wformat-truncation
  -Wmissing-prototypes
  -Wundef
  -fno-common)

add_library(
  ${CMAKE_PROJECT_NAME} SHARED
  ${CMAKE_SOURCE_DIR}/src/${CMAKE_PROJECT_NAME}.c
  ${CMAKE_SOURCE_DIR}/src/string_utils.h ${CMAKE_SOURCE_DIR}/src/string_utils.c)

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES C_STANDARD 11 PREFIX "")

install(
  TARGETS ${CMAKE_PROJECT_NAME}
  LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX})
