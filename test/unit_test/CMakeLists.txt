###############################
#   Prepare dependent tools   #
###############################

# find lcov tool for coverage data.
if(FK_YAML_CODE_COVERAGE)
  find_program(LCOV_TOOL NAMES lcov REQUIRED)
  execute_process(
    COMMAND ${LCOV_TOOL} --version
    OUTPUT_VARIABLE LCOV_TOOL_VERSION
    ERROR_VARIABLE LCOV_TOOL_VERSION)
  string(REGEX MATCH "[0-9]+(\\.[0-9]+)+" LCOV_TOOL_VERSION "${LCOV_TOOL_VERSION}")
  message(STATUS "lcov ${LCOV_TOOL_VERSION} (${LCOV_TOOL})")
endif()

# find Valgrind & prepare to use it as the target memory check tool.
if(FK_YAML_RUN_VALGRIND)
  find_program(MEMORYCHECK_COMMAND valgrind REQUIRED)
  set(MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full --error-exitcode=1")
endif()

############################################
#   configure C++ standard for unit test   #
############################################

# determine C++ standard used in unit test app.
if("${FK_YAML_TEST_TARGET_CXX_STANDARD}" STREQUAL "")
  # Apply minimum required C++ standard check by default.
  set(FK_YAML_TEST_TARGET_CXX_STANDARD 11)
endif()

# detect C++ standards supported by the compiler.
foreach(feature ${CMAKE_CXX_COMPILE_FEATURES})
  if(${feature} STREQUAL cxx_std_11)
    set(compiler_supports_cxx_11 ON)
  elseif(${feature} STREQUAL cxx_std_14)
    set(compiler_supports_cxx_14 ON)
  elseif(${feature} STREQUAL cxx_std_17)
    set(compiler_supports_cxx_17 ON)
  elseif(${feature} STREQUAL cxx_std_20)
    set(compiler_supports_cxx_20 ON)
  elseif(${feature} STREQUAL cxx_std_23)
    set(compiler_supports_cxx_23 ON)
  endif()
endforeach()

# check if the determined C++ standard is supported by the compiler.
if(NOT compiler_supports_cxx_${FK_YAML_TEST_TARGET_CXX_STANDARD})
  message(WARNING "The target compiler does not support C++${FK_YAML_TEST_TARGET_CXX_STANDARD}. Stop configuring unit test app.")
  return()
endif()

######################################
#   Prepare to use test data files   #
######################################

# While executing CTest on Windows, just specifying a relative path seems not working.
# This is a workaround to resolve the issue. (Delete it when a better solution is found.)
file(WRITE  ${CMAKE_CURRENT_BINARY_DIR}/include/test_data.hpp "#ifndef FK_YAML_TEST_TEST_DATA_HPP_\n")
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/include/test_data.hpp "#define FK_YAML_TEST_TEST_DATA_HPP_\n\n")
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/include/test_data.hpp "#define FK_YAML_TEST_DATA_DIR \"${CMAKE_CURRENT_SOURCE_DIR}/test_data\"\n\n")
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/include/test_data.hpp "#endif /* FK_YAML_TEST_TEST_DATA_HPP_ */\n")

#################################
#   Configure compile options   #
#################################

add_library(unit_test_config INTERFACE)
target_include_directories(unit_test_config INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/include)
target_link_libraries(unit_test_config INTERFACE Catch2::Catch2 ${FK_YAML_TARGET_NAME})
target_compile_features(unit_test_config INTERFACE cxx_std_${FK_YAML_TEST_TARGET_CXX_STANDARD})
set_target_properties(unit_test_config PROPERTIES CXX_EXTENTIONS OFF)
target_compile_definitions(unit_test_config INTERFACE
  $<$<CONFIG:Release>:NDEBUG>
  $<$<BOOL:${FK_YAML_USE_SINGLE_HEADER}>:FK_YAML_TEST_USE_SINGLE_HEADER>
)

# Configure compile options in common
target_compile_options(unit_test_config INTERFACE
  # MSVC
  $<$<CXX_COMPILER_ID:MSVC>:
    /W4 /EHsc /utf-8 /permissive-
    $<$<CONFIG:Debug>:/Z7>
    $<$<CONFIG:Release>:/Od>
  >

  # GNU
  $<$<CXX_COMPILER_ID:GNU>:
    -Wall -Wextra -pedantic -Wpedantic --all-warnings --extra-warnings
    -Wc++0x-compat
    -Wno-self-move # necessary to build iterator class test.
  >

  # Clang
  $<$<CXX_COMPILER_ID:Clang>:
    -Wall -pedantic -Wno-c++98-compat -Wno-c++98-compat-pedantic
    -Wno-self-assign-overloaded -Wno-self-move # necessary to build iterator class test.
  >
)

# additional compile options for Clang Sanitizers.
if(FK_YAML_RUN_CLANG_SANITIZERS)
  target_compile_options(
    unit_test_config
    INTERFACE -O1
              -g
              -fno-omit-frame-pointer
              -fsanitize=address,undefined,bounds,integer,nullability
              -fno-sanitize-recover=all
              -fno-sanitize=unsigned-integer-overflow,unsigned-shift-base
  )

  if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.13)
    target_link_options(
      unit_test_config
      INTERFACE -fno-omit-frame-pointer
                -fsanitize=address,undefined,bounds,integer,nullability
                -fno-sanitize-recover=all
                -fno-sanitize=unsigned-integer-overflow,unsigned-shift-base
    )
  else()
    target_link_libraries(
      unit_test_config
      INTERFACE -fno-omit-frame-pointer
                -fsanitize=address,undefined,bounds,integer,nullability
                -fno-sanitize-recover=all
                -fno-sanitize=unsigned-integer-overflow,unsigned-shift-base
    )
  endif()
endif()

# additional compile options for coverage.
if(FK_YAML_CODE_COVERAGE)
  target_compile_options(
    unit_test_config
    INTERFACE -O0        # no optimization
              -g         # generate debug info
              --coverage # set all required flags to generate code coverage data
  )

  if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.13)
    target_link_options(unit_test_config INTERFACE --coverage)
  else()
    target_link_libraries(unit_test_config INTERFACE --coverage)
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
  test_encode_detector.cpp
  test_exception_class.cpp
  test_from_string.cpp
  test_input_adapter.cpp
  test_input_handler.cpp
  test_iterator_class.cpp
  test_lexical_analyzer_class.cpp
  test_node_class.cpp
  test_node_ref_storage_class.cpp
  test_ordered_map_class.cpp
  test_serializer_class.cpp
  test_utf8_encoding_class.cpp
  main.cpp)

target_link_libraries(${TEST_TARGET} PRIVATE unit_test_config)

include(Catch)
catch_discover_tests(${TEST_TARGET})

add_dependencies(${TEST_TARGET} ${FK_YAML_TARGET_NAME})

add_custom_command(
  TARGET ${TEST_TARGET} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/test_data $<TARGET_FILE_DIR:${TEST_TARGET}>/test_data
)

############################################
#   Configure custom target for coverage   #
############################################

if(FK_YAML_CODE_COVERAGE)
  file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/include/fkYAML/*.hpp)

  add_custom_target(
    generate_test_coverage
    COMMAND ${CMAKE_CTEST_COMMAND} -C ${CMAKE_BUILD_TYPE} --output-on-failure
    COMMAND cd ${PROJECT_BINARY_DIR}/test/unit_test/CMakeFiles/${TEST_TARGET}.dir
    COMMAND ${LCOV_TOOL} --directory . --capture --output-file ${PROJECT_NAME}.info --rc
            lcov_branch_coverage=1
    COMMAND ${LCOV_TOOL} -e ${PROJECT_NAME}.info ${SRC_FILES}
            --output-file ${PROJECT_NAME}.info.filtered --rc lcov_branch_coverage=1
    COMMAND ${CMAKE_SOURCE_DIR}/thirdparty/imapdl/filterbr.py ${PROJECT_NAME}.info.filtered >
            ${PROJECT_NAME}.info.filtered.noexcept
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_NAME}.info.filtered.noexcept
            ${PROJECT_BINARY_DIR}/coverage/fkYAML.info
    DEPENDS ${TEST_TARGET}
    COMMENT "Execute unit test app with code coverage."
  )
endif()
