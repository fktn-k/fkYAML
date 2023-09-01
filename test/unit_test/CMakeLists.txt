if(MSVC)
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc /utf-8 /permissive-")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Z7")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Od")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-terminate")
endif()
set(CMAKE_CXX_STANDARD 11)

add_sanitizer_flags(${FK_YAML_RunSanitizers})

set(TEST_TARGET "fkYAMLUnitTest")

if(${FK_YAML_RunClangFormat})
  run_clang_format(
    ${TEST_TARGET}
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
  )
endif()

add_executable(${TEST_TARGET}
  ExceptionClassTest.cpp
  NodeClassTest.cpp
  IteratorClassTest.cpp
  LexicalAnalyzerClassTest.cpp
  DeserializerClassTest.cpp
  main.cpp
)

if(FK_YAML_GenerateCoverage)
  target_compile_options(${TEST_TARGET} PRIVATE
    -O0        # no optimization
    -g         # generate debug info
    --coverage # set all required flags to generate code coverage data
  )
  if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.13)
    target_link_options(${TEST_TARGET} PRIVATE --coverage)
  else()
    target_link_libraries(${TEST_TARGET} PRIVATE --coverage)
  endif()

  add_custom_target(generate_test_coverage
    COMMAND ${CMAKE_CTEST_COMMAND} -C ${CMAKE_BUILD_TYPE} --output-on-failure
    COMMAND cd ${PROJECT_BINARY_DIR}/test/unit_test/CMakeFiles/${TEST_TARGET}.dir
    COMMAND ${LCOV_TOOL} --directory . --capture --output-file ${PROJECT_NAME}.info --rc lcov_branch_coverage=1
    COMMAND ${LCOV_TOOL} -e ${PROJECT_NAME}.info ${PROJECT_SOURCE_DIR}/include/fkYAML/*.hpp --output-file ${PROJECT_NAME}.info.filtered --rc lcov_branch_coverage=1
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_NAME}.info.filtered ${PROJECT_BINARY_DIR}/coverage/fkYAML.info

    DEPENDS ${TEST_TARGET}
    COMMENT "Execute unit test app with code coverage."
  )
endif()

target_include_directories(${TEST_TARGET} PRIVATE
  ${PROJECT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${TEST_TARGET}
  Catch2::Catch2
)

include(Catch)
catch_discover_tests(${TEST_TARGET})
