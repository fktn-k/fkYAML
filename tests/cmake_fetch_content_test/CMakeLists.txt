add_test(
  NAME cmake_fetch_content_configure
  COMMAND
    ${CMAKE_COMMAND}
      -G "${CMAKE_GENERATOR}"
      -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
      -S${CMAKE_CURRENT_SOURCE_DIR}/project
      -B${CMAKE_CURRENT_BINARY_DIR}/build
)

add_test(
  NAME cmake_fetch_content_build
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_CURRENT_BINARY_DIR}/build
)
set_tests_properties(
  cmake_fetch_content_build
  PROPERTIES
    DEPENDS cmake_fetch_content_configure
)
