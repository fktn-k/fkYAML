
################################
#   Set up benchmarking tool   #
################################

include(FetchContent)

# Google benchmark
# disable building unit tests.
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
  gbench
  URL https://github.com/google/benchmark/archive/refs/tags/v1.9.5.zip
)
FetchContent_MakeAvailable(gbench)

###########################
#   Set up YAML parsers   #
###########################

# libfyaml
# build statically so that no shared library has to be found at run time, and disable building
# its tests, which would download the YAML and JSON test suites.
set(BUILD_SHARED_LIBS OFF)
set(BUILD_TESTING OFF)
FetchContent_Declare(
  libfyaml
  GIT_REPOSITORY https://github.com/pantoniou/libfyaml
  GIT_TAG v1.0.0-beta1
)
FetchContent_MakeAvailable(libfyaml)

# rapidyaml
FetchContent_Declare(
  rapidyaml
  GIT_REPOSITORY https://github.com/biojppm/rapidyaml
  GIT_TAG v0.16.0
)
FetchContent_MakeAvailable(rapidyaml)

# yaml-cpp
set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
  yaml_cpp
  GIT_REPOSITORY https://github.com/jbeder/yaml-cpp
  GIT_TAG yaml-cpp-0.9.0
)
FetchContent_MakeAvailable(yaml_cpp)

###############################
#   Set up benchmarking app   #
###############################

add_executable(
  benchmarker
  main.cpp
)

target_compile_options(
  benchmarker
  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:
      $<$<CONFIG:Debug>:/O1>
      $<$<CONFIG:Release>:/O2>
    >

    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:
      $<$<CONFIG:Debug>:-O1>
      $<$<CONFIG:Release>:-O2>
    >
)

target_compile_options(
  ryml
  PUBLIC
    $<$<CXX_COMPILER_ID:MSVC>: /wd4819>
)

target_link_libraries(
  benchmarker
  PRIVATE
    fkYAML::fkYAML
    fyaml
    ryml::ryml
    yaml-cpp::yaml-cpp
    benchmark::benchmark
)
