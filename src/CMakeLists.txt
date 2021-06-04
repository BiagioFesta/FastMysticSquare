include(FetchContent)

FetchContent_Declare(
  cxxopts
  GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
  GIT_TAG v2.2.1)
set(CXXOPTS_BUILD_EXAMPLES
    OFF
    CACHE BOOL "" FORCE)
set(CXXOPTS_BUILD_TESTS
    OFF
    CACHE BOOL "" FORCE)
set(CXXOPTS_ENABLE_INSTALL
    OFF
    CACHE BOOL "" FORCE)
set(CXXOPTS_ENABLE_WARNINGS
    OFF
    CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(cxxopts)

find_package(Threads)

add_executable(${PROJECT_NAME} DistanceManhattan.cpp Kpuzzle4.cpp SearchNode.cpp State.cpp)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_link_libraries(${PROJECT_NAME} PRIVATE cxxopts Threads::Threads)
install(TARGETS ${PROJECT_NAME})
