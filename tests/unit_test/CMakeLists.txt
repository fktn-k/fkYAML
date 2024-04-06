###############################
#   Prepare dependent tools   #
###############################

# find lcov tool for coverage data.
if(FK_YAML_CODE_COVERAGE)
  find_program(LCOV_TOOL NAMES lcov REQUIRED)
  execute_process(
    COMMAND ${LCOV_TOOL} --version
    OUTPUT_VARIABLE LCOV_TOOL_VERSION
    ERROR_VARIABLE LCOV_TOOL_VERSION
  )
  string(REGEX MATCH "[0-9]+(\\.[0-9]+)+" LCOV_TOOL_VERSION "${LCOV_TOOL_VERSION}")
  message(STATUS "Found lcov. version: ${LCOV_TOOL_VERSION} path: ${LCOV_TOOL}")
endif()

######################################
#   Prepare to use test data files   #
######################################

# While executing CTest on Windows, just specifying a relative path seems not working.
# This is a workaround to resolve the issue. (Delete it when a better solution is found.)
set(TEST_DATA_HEADER "${CMAKE_CURRENT_BINARY_DIR}/include/test_data.hpp")
file(WRITE  ${TEST_DATA_HEADER} "#ifndef FK_YAML_TEST_TEST_DATA_HPP\n")
file(APPEND ${TEST_DATA_HEADER} "#define FK_YAML_TEST_TEST_DATA_HPP\n\n")
file(APPEND ${TEST_DATA_HEADER} "#define FK_YAML_TEST_DATA_DIR \"${CMAKE_CURRENT_SOURCE_DIR}/test_data\"\n\n")
file(APPEND ${TEST_DATA_HEADER} "#endif /* FK_YAML_TEST_TEST_DATA_HPP */\n")

###########################
#   Build unit test app   #
###########################

add_executable(
  fkyaml_unit_tests
  test_custom_from_node.cpp
  test_deserializer_class.cpp
  test_exception_class.cpp
  test_input_adapter.cpp
  test_iterator_class.cpp
  test_lexical_analyzer_class.cpp
  test_node_attrs.cpp
  test_node_class.cpp
  test_node_ref_storage_class.cpp
  test_node_type.cpp
  test_ordered_map_class.cpp
  test_position_tracker_class.cpp
  test_reverse_iterator_class.cpp
  test_scalar_conv.cpp
  test_scalar_parser_class.cpp
  test_scalar_scanner_class.cpp
  test_serializer_class.cpp
  test_str_view_class.cpp
  test_string_formatter.cpp
  test_tag_resolver_class.cpp
  test_uri_encoding_class.cpp
  test_utf_encode_detector.cpp
  test_utf_encodings.cpp
  test_yaml_escaper_class.cpp
  test_yaml_version_type.cpp
)
target_link_libraries(fkyaml_unit_tests PRIVATE test_main)
target_include_directories(fkyaml_unit_tests PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/include")
# Configure compile options according to the target compiler.
target_compile_options(
  fkyaml_unit_tests
  PRIVATE
    # GNU
    $<$<CXX_COMPILER_ID:GNU>:
      -Wno-self-move # necessary to build the detail::iterator class test
    >
)

# additional compile options for coverage.
if(FK_YAML_CODE_COVERAGE)
  target_compile_options(
    fkyaml_unit_tests
    PRIVATE
      -O0        # no optimization
      -g         # generate debug info
      --coverage # set all required flags to generate code coverage data
  )

  if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.13)
    target_link_options(
      fkyaml_unit_tests
      PRIVATE
        --coverage
    )
  else()
    target_link_libraries(
      fkyaml_unit_tests
        PRIVATE
          --coverage
    )
  endif()
endif()

catch_discover_tests(fkyaml_unit_tests)

############################################
#   Configure custom target for coverage   #
############################################

if(FK_YAML_CODE_COVERAGE)
  file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/include/fkYAML/*.hpp)

  add_custom_target(
    generate_test_coverage
    COMMAND ${CMAKE_CTEST_COMMAND} -C ${CMAKE_BUILD_TYPE} --output-on-failure
    COMMAND cd ${PROJECT_BINARY_DIR}/tests/unit_test/CMakeFiles/fkyaml_unit_tests.dir
    COMMAND ${LCOV_TOOL} --directory . --capture --output-file ${PROJECT_NAME}.info --rc lcov_branch_coverage=1
    COMMAND ${LCOV_TOOL} -e ${PROJECT_NAME}.info ${SRC_FILES} --output-file ${PROJECT_NAME}.info.filtered --rc lcov_branch_coverage=1
    COMMAND ${PROJECT_SOURCE_DIR}/thirdparty/imapdl/filterbr.py ${PROJECT_NAME}.info.filtered > ${PROJECT_NAME}.info.filtered.noexcept
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_NAME}.info.filtered.noexcept ${PROJECT_BINARY_DIR}/coverage/fkYAML.info
    DEPENDS fkyaml_unit_tests
    COMMENT "Execute unit test app with code coverage."
  )
endif()
