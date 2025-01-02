cmake_minimum_required(VERSION 3.8...3.10)

project(CMakeFindPackageTestProject)

find_package(fkYAML REQUIRED)

add_executable(
  CMakeFindPackageTest
  main.cpp
)
target_link_libraries(
  CMakeFindPackageTest
  PUBLIC
    fkYAML::fkYAML
)
