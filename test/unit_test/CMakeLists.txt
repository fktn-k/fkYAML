if(FK_YAML_CODE_COVERAGE)
  find_program(LCOV_TOOL NAMES lcov REQUIRED)
  execute_process(
    COMMAND ${LCOV_TOOL} --version
    OUTPUT_VARIABLE LCOV_TOOL_VERSION
    ERROR_VARIABLE LCOV_TOOL_VERSION)
  string(REGEX MATCH "[0-9]+(\\.[0-9]+)+" LCOV_TOOL_VERSION "${LCOV_TOOL_VERSION}")
  message(STATUS "lcov ${LCOV_TOOL_VERSION} (${LCOV_TOOL})")
endif()

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

if("${FK_YAML_TEST_TARGET_CXX_STANDARD}" STREQUAL "")
  # Apply minimum required C++ standard check by default.
  set(FK_YAML_TEST_TARGET_CXX_STANDARD 11)
endif()

set(TEST_TARGET "fkYAMLUnitTest")

add_executable(
  ${TEST_TARGET}
  test_deserializer_class.cpp
  test_exception_class.cpp
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
set_target_properties(${TEST_TARGET} PROPERTIES CXX_STANDARD ${FK_YAML_TEST_TARGET_CXX_STANDARD}
                                                CXX_STANDARD_REQUIRED ON)

include(Catch)
catch_discover_tests(${TEST_TARGET})

add_dependencies(${TEST_TARGET} ${FK_YAML_TARGET_NAME})

if(${FK_YAML_RUN_CLANG_FORMAT})
  run_clang_format(${TEST_TARGET} "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
endif()

if(FK_YAML_RUN_VALGRIND)
  find_program(MEMORYCHECK_COMMAND valgrind REQUIRED)
  set(MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full --error-exitcode=1")
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

  add_custom_target(
    generate_test_coverage
    COMMAND ${CMAKE_CTEST_COMMAND} -C ${CMAKE_BUILD_TYPE} --output-on-failure
    COMMAND cd ${PROJECT_BINARY_DIR}/test/unit_test/CMakeFiles/${TEST_TARGET}.dir
    COMMAND ${LCOV_TOOL} --directory . --capture --output-file ${PROJECT_NAME}.info --rc
            lcov_branch_coverage=1
    COMMAND ${LCOV_TOOL} -e ${PROJECT_NAME}.info ${PROJECT_SOURCE_DIR}/include/fkYAML/*.hpp
            --output-file ${PROJECT_NAME}.info.filtered --rc lcov_branch_coverage=1
    COMMAND ${CMAKE_SOURCE_DIR}/thirdparty/imapdl/filterbr.py ${PROJECT_NAME}.info.filtered >
            ${PROJECT_NAME}.info.filtered.noexcept
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_NAME}.info.filtered.noexcept
            ${PROJECT_BINARY_DIR}/coverage/fkYAML.info
    DEPENDS ${TEST_TARGET}
    COMMENT "Execute unit test app with code coverage.")
endif()
