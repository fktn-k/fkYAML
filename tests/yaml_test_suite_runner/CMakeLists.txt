###########################
#   Download test suite   #
###########################

include(FetchContent)
FetchContent_Populate(
  yaml-test-suite
  GIT_REPOSITORY https://github.com/yaml/yaml-test-suite.git
  GIT_TAG data-2022-01-17)

# Configure the header file to embed the yaml-test-suite directory path
# for this environment.
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/yaml_test_suite_dir.hpp.in"
  "${CMAKE_CURRENT_BINARY_DIR}/include/yaml_test_suite_dir.hpp"
  @ONLY
)

###############################
#   Build test suite runner   #
###############################

add_executable(
  yaml_test_suite
  test_deserialize.cpp
)
target_link_libraries(yaml_test_suite PRIVATE test_main)
target_include_directories(yaml_test_suite PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/include")

catch_discover_tests(yaml_test_suite)
