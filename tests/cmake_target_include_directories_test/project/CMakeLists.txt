cmake_minimum_required(VERSION 3.8...3.10)

project(CMakeTargetIncludeDirectoriesTestProject)

add_executable(
  CMakeTargetIncludeDirectoriesTest
  main.cpp
)
target_include_directories(
  CMakeTargetIncludeDirectoriesTest
  PUBLIC
    ${fkYAML_source_dir}/include
)
set_target_properties(
  CMakeTargetIncludeDirectoriesTest
  PROPERTIES
    CXX_STANDARD 11
)
