cmake_minimum_required(VERSION 3.8...3.10)

project(CMakeAddSubDirectoryTestProject)

add_subdirectory(${fkYAML_source_dir} ${CMAKE_CURRENT_BINARY_DIR}/fkYAML)

add_executable(
  CMakeAddSubdirectoryWithNamespaceTest
  main.cpp
)
target_link_libraries(
  CMakeAddSubdirectoryWithNamespaceTest
  PUBLIC
    fkYAML::fkYAML
)

add_executable(
  CMakeAddSubdirectoryWithoutNamespaceTest
  main.cpp
)
target_link_libraries(
  CMakeAddSubdirectoryWithoutNamespaceTest
  PUBLIC
    fkYAML
)
