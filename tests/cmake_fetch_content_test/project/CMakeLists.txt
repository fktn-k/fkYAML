cmake_minimum_required(VERSION 3.11)

project("CMakeFetchContentTestProject")

include(FetchContent)
FetchContent_Declare(
  fkYAML
  GIT_REPOSITORY https://github.com/fktn-k/fkYAML.git
  GIT_TAG v0.4.2)
FetchContent_MakeAvailable(fkYAML)

add_executable(
  CMakeFetchContentTest
  main.cpp
)
target_link_libraries(
  CMakeFetchContentTest
  PUBLIC
    fkYAML::fkYAML
)
