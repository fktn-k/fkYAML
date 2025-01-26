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

# find Valgrind & prepare to use it as the target memory check tool.
if(FK_YAML_RUN_VALGRIND)
  find_program(MEMORYCHECK_COMMAND valgrind REQUIRED)
  execute_process(
    COMMAND ${MEMORYCHECK_COMMAND} --version
    OUTPUT_VARIABLE VALGRIND_VERSION
    ERROR_VARIABLE  VALGRIND_VERSION
  )
  set(REGEX MATCH "[0-9]+(\\.[0-9]+)+" VALGRIND_VERSION ${VALGRIND_VERSION})
  message(STATUS "Found Valgrind. version: ${VALGRIND_VERSION} path: ${MEMORYCHECK_COMMAND}")
  set(MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full --error-exitcode=1")
endif()

############################################
#   configure C++ standard for unit test   #
############################################

# determine C++ standard used in unit test app.
if("${FK_YAML_TEST_TARGET_CXX_STANDARD}" STREQUAL "")
  # Apply minimum required C++ standard checked by default.
  set(FK_YAML_TEST_TARGET_CXX_STANDARD 11)
endif()

# detect C++ standards supported by the compiler.
foreach(feature ${CMAKE_CXX_COMPILE_FEATURES})
  if(${feature} STREQUAL cxx_std_11)
    set(FK_YAML_COMPILER_SUPPORTS_CXX_11 ON)
  elseif(${feature} STREQUAL cxx_std_14)
    set(FK_YAML_COMPILER_SUPPORTS_CXX_14 ON)
  elseif(${feature} STREQUAL cxx_std_17)
    set(FK_YAML_COMPILER_SUPPORTS_CXX_17 ON)
  elseif(${feature} STREQUAL cxx_std_20)
    set(FK_YAML_COMPILER_SUPPORTS_CXX_20 ON)
  elseif(${feature} STREQUAL cxx_std_23)
    set(FK_YAML_COMPILER_SUPPORTS_CXX_23 ON)
  endif()
endforeach()

# check if the determined C++ standard is supported by the compiler.
if(NOT FK_YAML_COMPILER_SUPPORTS_CXX_${FK_YAML_TEST_TARGET_CXX_STANDARD})
  message(WARNING "The target compiler does not support C++${FK_YAML_TEST_TARGET_CXX_STANDARD}. Stop configuring unit test app.")
  return()
endif()

######################################
#   Prepare to use test data files   #
######################################

# While executing CTest on Windows, just specifying a relative path seems not working.
# This is a workaround to resolve the issue. (Delete it when a better solution is found.)
set(TEST_DATA_HEADER "${CMAKE_CURRENT_BINARY_DIR}/include/test_data.hpp")
file(WRITE  ${TEST_DATA_HEADER} "#ifndef FK_YAML_TEST_TEST_DATA_HPP_\n")
file(APPEND ${TEST_DATA_HEADER} "#define FK_YAML_TEST_TEST_DATA_HPP_\n\n")
file(APPEND ${TEST_DATA_HEADER} "#define FK_YAML_TEST_DATA_DIR \"${CMAKE_CURRENT_SOURCE_DIR}/test_data\"\n\n")
file(APPEND ${TEST_DATA_HEADER} "#endif /* FK_YAML_TEST_TEST_DATA_HPP_ */\n")

#################################
#   Configure compile options   #
#################################

add_library(unit_test_config INTERFACE)
target_include_directories(
  unit_test_config
  INTERFACE
    "${CMAKE_CURRENT_BINARY_DIR}/include"
    "${CATCH2_ROOT_DIR}/single_include"
)
target_link_libraries(
  unit_test_config
  INTERFACE
    ${FK_YAML_TARGET_NAME}
)
target_compile_features(
  unit_test_config
  INTERFACE
    cxx_std_${FK_YAML_TEST_TARGET_CXX_STANDARD}
)
set_target_properties(
  unit_test_config
  PROPERTIES
    CXX_EXTENTIONS OFF
)
target_compile_definitions(
  unit_test_config
  INTERFACE
    $<$<CONFIG:Release>:NDEBUG>
)

# Configure compile options according to the target compiler.
target_compile_options(
  unit_test_config
  INTERFACE
    # MSVC
    $<$<CXX_COMPILER_ID:MSVC>:
      /W4 /WX /EHsc /utf-8 /permissive-
      /wd4709 # comma operator within array index expression
      /wd4996 # for testing deprecated functions
      /w44800 # https://github.com/fktn-k/fkYAML/issues/429
      $<$<CONFIG:Debug>:/Z7>
      $<$<CONFIG:Release>:/Od>
    >
    # GNU
    $<$<CXX_COMPILER_ID:GNU>:
      -Wall -Wextra -Werror -pedantic -Wpedantic -Wdeprecated --all-warnings --extra-warnings
      -Wno-deprecated-declarations # for testing deprecated functions
      -Wno-self-move # necessary to build the detail::iterator class test
    >
    # Clang
    $<$<CXX_COMPILER_ID:Clang>:
      -Wall -Wextra -Werror -pedantic -Wdeprecated
      -Wno-c++98-compat -Wno-c++98-compat-pedantic
      -Wno-deprecated-declarations # for testing deprecated functions
    >
    $<$<CXX_COMPILER_ID:AppleClang>:
      -Wall -Wextra -Werror -pedantic -Wdeprecated
      -Wno-deprecated-declarations # for testing deprecated functions
    >
    # IntelLLVM
    $<$<CXX_COMPILER_ID:IntelLLVM>:
      # IntelLLVM warns the usage of nans and infinities due to its over-estimation.
      # fkYAML, however, uses them as YAML node values, not as calculation.
      # To disable too aggressive warnings, `-fp-model=precise` is used in the test as a workaround.
      -fp-model=precise
      $<$<CONFIG:Debug>:-O0 -g>
      $<$<CONFIG:Release>:-O2>
    >
)

target_link_options(
  unit_test_config
  INTERFACE
    $<$<CXX_COMPILER_ID:IntelLLVM>:
      $<$<CONFIG:Debug>:-O0 -g>
      $<$<CONFIG:Release>:-O2>
    >
)

# additional compile options for Clang Sanitizers.
if(FK_YAML_RUN_CLANG_SANITIZERS)
  target_compile_options(
    unit_test_config
    INTERFACE
      -O0
      -g
      -fno-omit-frame-pointer
      -fsanitize=address,undefined,bounds,integer,nullability
      -fno-sanitize-recover=all
      -fno-sanitize=signed-integer-overflow,implicit-conversion
  )

  if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.13)
    target_link_options(
      unit_test_config
      INTERFACE
        -fno-omit-frame-pointer
        -fsanitize=address,undefined,bounds,integer,nullability
        -fno-sanitize-recover=all
        -fno-sanitize=signed-integer-overflow,implicit-conversion
    )
  else()
    target_link_libraries(
      unit_test_config
      INTERFACE
        -fno-omit-frame-pointer
        -fsanitize=address,undefined,bounds,integer,nullability
        -fno-sanitize-recover=all
        -fno-sanitize=signed-integer-overflow,implicit-conversion
    )
  endif()
endif()

# additional compile options for coverage.
if(FK_YAML_CODE_COVERAGE)
  target_compile_options(
    unit_test_config
    INTERFACE
      -O0        # no optimization
      -g         # generate debug info
      --coverage # set all required flags to generate code coverage data
  )

  if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.13)
    target_link_options(
      unit_test_config
      INTERFACE
        --coverage
    )
  else()
    target_link_libraries(
      unit_test_config
        INTERFACE
          --coverage
    )
  endif()
endif()

###########################
#   Build unit test app   #
###########################

set(TEST_TARGET "fkYAMLUnitTest")

add_executable(
  ${TEST_TARGET}
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
  main.cpp
)

target_link_libraries(${TEST_TARGET} PRIVATE unit_test_config)

include(Catch)
catch_discover_tests(${TEST_TARGET})

add_dependencies(${TEST_TARGET} ${FK_YAML_TARGET_NAME})

add_custom_command(
  TARGET ${TEST_TARGET}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/test_data" "$<TARGET_FILE_DIR:${TEST_TARGET}>/test_data"
)

############################################
#   Configure custom target for coverage   #
############################################

if(FK_YAML_CODE_COVERAGE)
  file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/include/fkYAML/*.hpp)

  add_custom_target(
    generate_test_coverage
    COMMAND ${CMAKE_CTEST_COMMAND} -C ${CMAKE_BUILD_TYPE} --output-on-failure
    COMMAND cd ${PROJECT_BINARY_DIR}/tests/unit_test/CMakeFiles/${TEST_TARGET}.dir
    COMMAND ${LCOV_TOOL} --directory . --capture --output-file ${PROJECT_NAME}.info --rc lcov_branch_coverage=1
    COMMAND ${LCOV_TOOL} -e ${PROJECT_NAME}.info ${SRC_FILES} --output-file ${PROJECT_NAME}.info.filtered --rc lcov_branch_coverage=1
    COMMAND ${PROJECT_SOURCE_DIR}/thirdparty/imapdl/filterbr.py ${PROJECT_NAME}.info.filtered > ${PROJECT_NAME}.info.filtered.noexcept
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_NAME}.info.filtered.noexcept ${PROJECT_BINARY_DIR}/coverage/fkYAML.info
    DEPENDS ${TEST_TARGET}
    COMMENT "Execute unit test app with code coverage."
  )
endif()
