###########################
#   Download test suite   #
###########################

include(FetchContent)
FetchContent_Populate(
  yaml-test-suite
  GIT_REPOSITORY https://github.com/yaml/yaml-test-suite.git
  GIT_TAG data-2022-01-17
)

#############################################################################
#   Configure the header file to embed the yaml-test-suite directory path   #
#############################################################################

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/yaml_test_suite_dir.hpp.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/yaml_test_suite_dir.hpp"
  @ONLY
)

###############################################################################
#   Generate test cases for each test case directory in the yaml-test-suite   #
###############################################################################

file(GLOB_RECURSE YAML_TEST_SUITE_EVENT_FILES
  RELATIVE "${yaml-test-suite_SOURCE_DIR}"
  "${yaml-test-suite_SOURCE_DIR}/*/test.event")
list(SORT YAML_TEST_SUITE_EVENT_FILES)

set(GENERATED_YAML_TEST_SUITE_CASES_HEADER
  "${CMAKE_CURRENT_BINARY_DIR}/generated_yaml_test_suite_cases.hpp")
set(GENERATED_YAML_TEST_SUITE_CASES_HEADER_TEMPLATE
  "${CMAKE_CURRENT_SOURCE_DIR}/generated_yaml_test_suite_cases.hpp.in")
set(YAML_TEST_SUITE_CASES_CONTENTS "")

foreach(event_file ${YAML_TEST_SUITE_EVENT_FILES})
  get_filename_component(case_dir "${event_file}" DIRECTORY)
  if(case_dir STREQUAL "tags" OR case_dir MATCHES "^tags/")
    continue()
  endif()

  string(REPLACE "/" "-" test_id "${case_dir}")
  string(APPEND YAML_TEST_SUITE_CASES_CONTENTS
    "TEST_CASE(\"yaml_test_suite_${test_id}\") { run_yaml_test_suite_case(\"${case_dir}\", \"${test_id}\"); }\n")
endforeach()

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/generated_yaml_test_suite_cases.hpp.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/generated_yaml_test_suite_cases.hpp"
  @ONLY
)

###############################
#   Build test suite runner   #
###############################

add_executable(
  yaml_test_suite
  test_deserialize.cpp
  runner_impls/tests/test_validation_compositor.cpp
)
target_link_libraries(
  yaml_test_suite
  PRIVATE
    common_test_config
)
target_include_directories(
  yaml_test_suite
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/runner_impls"
)

doctest_discover_tests(yaml_test_suite)
