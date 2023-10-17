# find lcov tool.
if(FK_YAML_CODE_COVERAGE)
  find_program(LCOV_TOOL NAMES lcov REQUIRED)
  execute_process(
    COMMAND ${LCOV_TOOL} --version
    OUTPUT_VARIABLE LCOV_TOOL_VERSION
    ERROR_VARIABLE LCOV_TOOL_VERSION)
  string(REGEX MATCH "[0-9]+(\\.[0-9]+)+" LCOV_TOOL_VERSION "${LCOV_TOOL_VERSION}")
  message(STATUS "lcov ${LCOV_TOOL_VERSION} (${LCOV_TOOL})")
endif()

# set compiler options for unit test app.
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc /utf-8 /permissive-")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Z7")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Od")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wno-terminate")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-c++98-compat -Wno-c++98-compat-pedantic")
endif()

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

set(TEST_TARGET "fkYAMLUnitTest")

add_executable(
  ${TEST_TARGET}
  test_deserializer_class.cpp
  test_exception_class.cpp
  test_from_string.cpp
  test_input_handler.cpp
  test_iterator_class.cpp
  test_lexical_analyzer_class.cpp
  test_node_class.cpp
  test_ordered_map_class.cpp
  test_serializer_class.cpp
  main.cpp)

target_include_directories(${TEST_TARGET} PRIVATE ${PROJECT_SOURCE_DIR}/include
                                                  ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${TEST_TARGET} Catch2::Catch2)

# set and require the target C++ standard. (defaults to C++11)
target_compile_features(${TEST_TARGET} PRIVATE cxx_std_${FK_YAML_TEST_TARGET_CXX_STANDARD})

set_target_properties(${TEST_TARGET} PROPERTIES CXX_EXTENTIONS OFF)

include(Catch)
catch_discover_tests(${TEST_TARGET})

add_dependencies(${TEST_TARGET} ${FK_YAML_TARGET_NAME})

# configure for Valgrind.
if(FK_YAML_RUN_VALGRIND)
  find_program(MEMORYCHECK_COMMAND valgrind REQUIRED)
  set(MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full --error-exitcode=1")
endif()

# configure for Clang Sanitizers.
if(FK_YAML_RUN_CLANG_SANITIZERS)
  target_compile_options(
    ${TEST_TARGET}
    PRIVATE -O1
            -g
            -fno-omit-frame-pointer
            -fsanitize=address,undefined,bounds,integer,nullability
            -fno-sanitize-recover=all
            -fno-sanitize=unsigned-integer-overflow,unsigned-shift-base
    )
  target_link_options(
    ${TEST_TARGET}
    PRIVATE -fno-omit-frame-pointer
            -fsanitize=address,undefined,bounds,integer,nullability
            -fno-sanitize-recover=all
            -fno-sanitize=unsigned-integer-overflow,unsigned-shift-base
  )
endif()

if(FK_YAML_CODE_COVERAGE)
  target_compile_options(
    ${TEST_TARGET}
    PRIVATE -O0 # no optimization
            -g # generate debug info
            --coverage # set all required flags to generate code coverage data
    )
  if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.13)
    target_link_options(${TEST_TARGET} PRIVATE --coverage)
  else()
    target_link_libraries(${TEST_TARGET} PRIVATE --coverage)
  endif()

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
    COMMENT "Execute unit test app with code coverage.")
endif()
