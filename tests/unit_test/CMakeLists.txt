######################################
#   Prepare to use test data files   #
######################################

# While executing CTest on Windows, just specifying a relative path seems not working.
# So, define the absolute path in this file to open test data files.
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/test_data.hpp.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/test_data.hpp"
  @ONLY
)

#################################
#   Configure compile options   #
#################################

add_library(unit_test_config INTERFACE)
target_include_directories(
  unit_test_config
  INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${doctest_SOURCE_DIR}"
)

# Configure extra compile options for unit tests according to the target compiler.
target_compile_options(
  unit_test_config
  INTERFACE
    $<$<CXX_COMPILER_ID:GNU>:
      -Wno-self-move # necessary to build the detail::iterator class test
    >
)

###########################
#   Build unit test app   #
###########################

add_executable(
  unit_test
  test_custom_from_node.cpp
  test_deserializer_class.cpp
  test_exception_class.cpp
  test_fuzz_regression.cpp
  test_input_adapter.cpp
  test_iterator_class.cpp
  test_lexical_analyzer_class.cpp
  test_node_attrs.cpp
  test_node_attrs_uint.cpp
  test_node_class.cpp
  test_node_class_uint.cpp
  test_node_ref_storage_class.cpp
  test_node_type.cpp
  test_ordered_map_class.cpp
  test_position_tracker_class.cpp
  test_reverse_iterator_class.cpp
  test_scalar_conv.cpp
  test_scalar_parser_class.cpp
  test_scalar_parser_class_uint.cpp
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
target_compile_options(
  unit_test
  PRIVATE
    # GNU
    $<$<CXX_COMPILER_ID:GNU>:
      -Wno-self-move # necessary to build the detail::iterator class test
    >
)
target_link_libraries(
  unit_test
  PRIVATE
    common_test_config_with_main
    unit_test_config
)
doctest_discover_tests(unit_test)
