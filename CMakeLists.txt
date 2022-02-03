cmake_minimum_required(VERSION 3.9)

project(
  spank-report
  VERSION 1.0.0
  DESCRIPTION "SPANK reporting plugin"
  LANGUAGES C)

# Availables options
set(SLURM_INCLUDE_DIR
    /usr/include
    CACHE PATH "Slurm include directory")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib64)
set(CMAKE_STATIC_LIBRARY_PREFIX "")
set(THREADS_PREFER_PTHREAD_FLAG ON)
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX
      /usr/lib64/slurm
      CACHE PATH "Install path" FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

find_package(Threads REQUIRED)
find_package(CURL REQUIRED)

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
  -Wmissing-prototypes
  -Wundef
  -fno-common)

file(GLOB_RECURSE sources src/*.c src/*.h)

add_library(spank_report SHARED ${sources})
target_link_libraries(spank_report PRIVATE Threads::Threads curl)
target_include_directories(spank_report PUBLIC ${SLURM_INCLUDE_DIR})
set_target_properties(spank_report PROPERTIES C_STANDARD 11 PREFIX "")

install(TARGETS spank_report LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX})

include(package.cmake)
