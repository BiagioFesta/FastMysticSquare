cmake_minimum_required(VERSION 3.14)
project(FastMysticSquare)

option(BUILD_TESTS "Build Tests" OFF)

add_subdirectory(${PROJECT_SOURCE_DIR}/src)

if(${BUILD_TESTS})
  add_subdirectory(${PROJECT_SOURCE_DIR}/tests)
endif()
