add_test(NAME cmake_target_include_directories_test_configure
  COMMAND ${CMAKE_COMMAND}
      -G "${CMAKE_GENERATOR}"
      -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
      -DfkYAML_source_dir=${PROJECT_SOURCE_DIR}
      -S${CMAKE_CURRENT_SOURCE_DIR}/project
      -B${CMAKE_CURRENT_BINARY_DIR}/build
)

add_test(NAME cmake_target_include_directories_test_build
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_CURRENT_BINARY_DIR}/build
)
set_tests_properties(cmake_target_include_directories_test_build PROPERTIES
  DEPENDS cmake_target_include_directories_test_configure
)
