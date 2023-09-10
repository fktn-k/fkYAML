# find necessary tools

foreach(COMPILER g++-9 g++-10 g++-11 g++-12 clang++-11 clang++-12 clang++-13 clang++-14 clang++-15)
  find_program(COMPILER_TOOL NAMES ${COMPILER})
  if(COMPILER_TOOL)
    add_custom_target(ci_test_compiler_${COMPILER}
      COMMAND CXX=${COMPILER} ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug -GNinja -DFK_YAML_CI=ON
        -S${PROJECT_SOURCE_DIR} -B${PROJECT_BINARY_DIR}/build_compiler_${COMPILER}
      COMMAND ${CMAKE_COMMAND} --build ${PROJECT_BINARY_DIR}/build_compiler_${COMPILER}
      COMMAND cd ${PROJECT_BINARY_DIR}/build_compiler_${COMPILER} && ${CMAKE_CTEST_COMMAND} -C Debug --output-on-failure
      COMMENT "Compile and test with ${COMPILER}"
    )
  endif()
  unset(COMPILER_TOOL CACHE)
endforeach()