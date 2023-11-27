# This minimum version is newer than fkYAML library's. But building this directory occurs only
# during CI. So, it should be fine.
cmake_minimum_required(VERSION 3.11.0)

project("CMakeFetchContentTestProject")

include(FetchContent)
FetchContent_Declare(
  fkYAML
  GIT_REPOSITORY https://github.com/fktn-k/fkYAML.git
  GIT_TAG v0.2.2)
FetchContent_MakeAvailable(fkYAML)

add_executable(CMakeFetchContentTest main.cpp)
target_link_libraries(CMakeFetchContentTest PUBLIC fkYAML::fkYAML)
