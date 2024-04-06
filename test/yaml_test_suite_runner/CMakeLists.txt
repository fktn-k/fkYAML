###########################
#   Download test suite   #
###########################

include(FetchContent)
FetchContent_Populate(
  yaml-test-suite
  GIT_REPOSITORY https://github.com/yaml/yaml-test-suite.git
  GIT_TAG data-2022-01-17)

###############################
#   Build test suite runner   #
###############################

add_executable(YamlTestSuiteRunner main.cpp skip_test_case_list.hpp)
target_link_libraries(YamlTestSuiteRunner ${FK_YAML_TARGET_NAME})
target_compile_options(YamlTestSuiteRunner PRIVATE
  # MSVC
  $<$<CXX_COMPILER_ID:MSVC>:
    /W4 /EHsc /utf-8 /permissive-
    $<$<CONFIG:Debug>:/Z7>
    $<$<CONFIG:Release>:/Od>
  >

  # GNU
  $<$<CXX_COMPILER_ID:GNU>:
    -Wall -Wextra -pedantic -Wpedantic --all-warnings --extra-warnings
  >

  # Clang
  $<$<CXX_COMPILER_ID:Clang>:
    -Wall -pedantic -Wno-c++98-compat -Wno-c++98-compat-pedantic
  >
)

#########################################
#   Register tests for each test data   #
#########################################

set(YAML_TEST_MODE 0)
set(JSON_TEST_MODE 1)

file(GLOB_RECURSE TEST_YAML_FILES ${yaml-test-suite_SOURCE_DIR}/*in.yaml)
foreach(TEST_YAML_FILE ${TEST_YAML_FILES})
  get_filename_component(TEST_CASE_DIR ${TEST_YAML_FILE} DIRECTORY)
  file(RELATIVE_PATH TEST_CASE_ID ${yaml-test-suite_SOURCE_DIR} ${TEST_CASE_DIR})
  string(REPLACE "/" "_" TEST_CASE_ID ${TEST_CASE_ID})
  add_test(NAME YamlTestSuite_${TEST_CASE_ID}_YAML COMMAND $<TARGET_FILE:YamlTestSuiteRunner> ${TEST_CASE_ID} ${TEST_CASE_DIR} ${YAML_TEST_MODE})

  if (EXISTS "${TEST_CASE_DIR}/in.json")
    add_test(NAME YamlTestSuite_${TEST_CASE_ID}_JSON COMMAND $<TARGET_FILE:YamlTestSuiteRunner> ${TEST_CASE_ID} ${TEST_CASE_DIR} ${JSON_TEST_MODE})
  endif()
endforeach()
