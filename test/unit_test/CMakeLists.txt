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
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
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
  NodeClassTest.cpp
  LexicalAnalyzerClassTest.cpp
  DeserializerClassTest.cpp
  main.cpp
)

target_include_directories(${TEST_TARGET} PRIVATE
  ${PROJECT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${TEST_TARGET}
  Catch2::Catch2
)

include(Catch)
catch_discover_tests(${TEST_TARGET})
