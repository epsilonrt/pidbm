cmake_minimum_required(VERSION 3.0.2)

## Use the variable PROJECT_NAME for changing the target name
set(PROJECT_NAME "pidbm")
set(LIB_TARGET "${PROJECT_NAME}")
set(BIN_TARGET "${PROJECT_NAME}-bin")

## Set our project name
project(${PROJECT_NAME})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(PIDUINODB_MAJOR 0)
set(PIDUINODB_MINOR 4)

# Appends the cmake/modules path to MAKE_MODULE_PATH variable.
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

find_package(CppDb REQUIRED)

include(GitVersion)
GetGitVersion(PIDBM_VERSION)
set(PIDBM_VERSION
  ${PIDBM_VERSION_MAJOR}.${PIDBM_VERSION_MINOR}.${PIDBM_VERSION_PATCH})
WriteGitVersionFile(${CMAKE_BINARY_DIR}/version.h)

include (CheckFunctionExists)
check_function_exists(getprogname HAVE_GETPROGNAME_FUNCTION)

include_directories(${CMAKE_SOURCE_DIR}/lib ${CMAKE_SOURCE_DIR}/3rdparty)
include_directories(${CMAKE_BINARY_DIR})
link_directories(${CMAKE_BINARY_DIR}/lib)

configure_file(
    "${PROJECT_SOURCE_DIR}/config.h.in"
    "${PROJECT_BINARY_DIR}/config.h")

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)
add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

add_subdirectory(lib)
add_subdirectory(main)
