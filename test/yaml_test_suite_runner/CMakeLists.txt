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

add_executable(YamlTestSuiteRunner main.cpp)
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

file(GLOB_RECURSE TEST_YAML_FILES ${yaml-test-suite_SOURCE_DIR}/*in.yaml)
foreach(TEST_YAML_FILE ${TEST_YAML_FILES})
  get_filename_component(TEST_CASE_DIR ${TEST_YAML_FILE} DIRECTORY)
  file(RELATIVE_PATH TEST_CASE_NAME ${yaml-test-suite_SOURCE_DIR} ${TEST_CASE_DIR})
  string(REPLACE "/" "_" TEST_CASE_NAME ${TEST_CASE_NAME})
  add_test(NAME YamlTestSuite_${TEST_CASE_NAME} COMMAND $<TARGET_FILE:YamlTestSuiteRunner> ${TEST_CASE_DIR})
endforeach()