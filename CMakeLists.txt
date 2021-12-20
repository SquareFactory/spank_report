cmake_minimum_required(VERSION 3.9)

# Availables options
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INSTALL_PREFIX /usr/lib/slurm)
set(SLURM_SRC_DIR ${CMAKE_SOURCE_DIR}/externals/slurm)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_STATIC_LIBRARY_PREFIX "")

project(
  jobcomp_report
  VERSION 1.0.0
  DESCRIPTION "Job completion reporting plugin")

add_compile_options(-O3 -Wall -Wextra -Wno-unused-parameter)

add_custom_command(
  OUTPUT ${SLURM_SRC_DIR}/slurm/slurm_version.h
  COMMAND ${SLURM_SRC_DIR}/configure
  WORKING_DIRECTORY ${SLURM_SRC_DIR}
  COMMENT "Configure slurm"
  VERBATIM)

add_custom_target(slurm_configure
                  DEPENDS ${SLURM_SRC_DIR}/slurm/slurm_version.h)

add_library(jobcomp_report SHARED ${CMAKE_SOURCE_DIR}/src/jobcomp_report.c)
add_library(jobcomp_report_static STATIC
            ${CMAKE_SOURCE_DIR}/src/jobcomp_report.c)

add_dependencies(jobcomp_report slurm_configure)
add_dependencies(jobcomp_report_static slurm_configure)

set_target_properties(jobcomp_report PROPERTIES C_STANDARD 11 PREFIX "")
set_target_properties(
  jobcomp_report_static
  PROPERTIES C_STANDARD 11
             PREFIX ""
             OUTPUT_NAME jobcomp_report)

target_include_directories(jobcomp_report PUBLIC ${SLURM_SRC_DIR})
target_include_directories(jobcomp_report_static PUBLIC ${SLURM_SRC_DIR})

install(
  TARGETS jobcomp_report jobcomp_report_static
  LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX})
