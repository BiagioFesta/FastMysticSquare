enable_testing()
include(GoogleTest)
include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG a3460d1aeeaa43fdf137a6adefef10ba0b59fe4b)
set(gtest_force_shared_crt
    ON
    CACHE BOOL "" FORCE)
set(BUILD_GMOCK
    ON
    CACHE BOOL "" FORCE)
set(INSTALL_GTEST
    OFF
    CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(
  ${PROJECT_NAME}_tests
  testAlgorithmIDA.cpp
  testDistanceManhattan.cpp
  testPatternDB.cpp
  testSearchNode.cpp
  testState.cpp
  ${PROJECT_SOURCE_DIR}/src/State.cpp
  ${PROJECT_SOURCE_DIR}/src/SearchNode.cpp
  ${PROJECT_SOURCE_DIR}/src/DistanceManhattan.cpp)
target_compile_features(${PROJECT_NAME}_tests PRIVATE cxx_std_17)
target_include_directories(${PROJECT_NAME}_tests PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(${PROJECT_NAME}_tests PRIVATE gtest gtest_main gmock)
gtest_add_tests(TARGET ${PROJECT_NAME}_tests)
