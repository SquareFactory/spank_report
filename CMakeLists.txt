cmake_minimum_required(VERSION 3.9)

# Availables options
set(SLURM_INCLUDE_DIR
    /usr/include
    CACHE PATH "Slurm include directory")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INSTALL_PREFIX /usr/lib/slurm)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_STATIC_LIBRARY_PREFIX "")
set(THREADS_PREFER_PTHREAD_FLAG ON)

project(
  spank_report
  VERSION 1.0.0
  DESCRIPTION "SPANK reporting plugin"
  LANGUAGES C)

find_package(Threads REQUIRED)

add_compile_options(
  -O3
  -Wno-unused-parameter
  -m64
  -std=c99
  -pedantic
  -Wall
  -Wextra
  -Werror
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

file(GLOB_RECURSE sources src/*.c src/*.h)

add_library(${CMAKE_PROJECT_NAME} SHARED ${sources})
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Threads::Threads)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${SLURM_INCLUDE_DIR})
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES C_STANDARD 11 PREFIX "")

install(TARGETS ${CMAKE_PROJECT_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX})
