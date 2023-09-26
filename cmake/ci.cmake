# find necessary tools (alphabetic order)

find_program(CLANGXX_TOOL NAMES clang++-15 clang++-14 clang++-13 clang++-12 clang++-11 clang++ REQUIRED)
execute_process(COMMAND ${CLANGXX_TOOL} --version OUTPUT_VARIABLE CLANGXX_TOOL_VERSION ERROR_VARIABLE CLANGXX_TOOL_VERSION)
string(REGEX MATCH "[0-9]+(\\.[0-9]+)+" CLANGXX_TOOL_VERSION "${CLANGXX_TOOL_VERSION}")
message(STATUS "# clang++ # version: ${CLANGXX_TOOL_VERSION} path: ${CLANGXX_TOOL}")

find_program(NINJA_TOOL NAMES ninja REQUIRED)
execute_process(COMMAND ${NINJA_TOOL} --version OUTPUT_VARIABLE NINJA_TOOL_VERSION ERROR_VARIABLE NINJA_TOOL_VERSION)
string(REGEX MATCH "[0-9]+(\\.[0-9]+)+" NINJA_TOOL_VERSION "${NINJA_TOOL_VERSION}")
message(STATUS "# Ninja # version: ${NINJA_TOOL_VERSION} path: ${NINJA_TOOL}")

#########################################################
#  Execute unit tests with various versions of clang++  #
#########################################################

foreach(COMPILER clang++-11 clang++-12 clang++-13 clang++-14 clang++-15)
  find_program(COMPILER_TOOL NAMES ${COMPILER})
  if(COMPILER_TOOL)
    add_custom_target(ci_test_compiler_${COMPILER}
      COMMAND CXX=${COMPILER} ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug -GNinja -DFK_YAML_BUILD_ALL_TEST=ON
        -S${PROJECT_SOURCE_DIR} -B${PROJECT_BINARY_DIR}/build_compiler_${COMPILER}
      COMMAND ${CMAKE_COMMAND} --build ${PROJECT_BINARY_DIR}/build_compiler_${COMPILER} --config Debug
      COMMAND cd ${PROJECT_BINARY_DIR}/build_compiler_${COMPILER} && ${CMAKE_CTEST_COMMAND} -C Debug --output-on-failure
      COMMENT "Compile and test with ${COMPILER}"
    )
  endif()
  unset(COMPILER_TOOL CACHE)
endforeach()

###############################################################
#  Execute unit tests with clang++ for various c++ standards  #
###############################################################

foreach(TARGET_CXX_STANDARD 11 14 17 20 23)
  add_custom_target(ci_test_clang++_c++${TARGET_CXX_STANDARD}
    COMMAND CXX=${CLANGXX_TOOL} ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug -GNinja -DFK_YAML_BUILD_ALL_TEST=ON
      -DFK_YAML_TEST_TARGET_CXX_STANDARD=${TARGET_CXX_STANDARD}
      -S${PROJECT_SOURCE_DIR} -B${PROJECT_BINARY_DIR}/build_clang++_c++${TARGET_CXX_STANDARD}
    COMMAND ${CMAKE_COMMAND} --build ${PROJECT_BINARY_DIR}/build_clang++_c++${TARGET_CXX_STANDARD} --config Debug
    COMMAND cd ${PROJECT_BINARY_DIR}/build_clang++_c++${TARGET_CXX_STANDARD} && ${CMAKE_CTEST_COMMAND} -C Debug --output-on-failure
    COMMENT "Compile and test with clang++ for C++${TARGET_CXX_STANDARD}"
  )
endforeach()

##############################################
#  Execute unit tests with clang sanitizers  #
##############################################

set(CLANGXX_SANITIZER_FLAGS
    "-g -O1 -fno-omit-frame-pointer -fsanitize=address,undefined,bounds,integer,nullability -fno-sanitize-recover=all -fno-sanitize=unsigned-integer-overflow,unsigned-shift-base")

add_custom_target(ci_test_clang++_sanitizer
  COMMAND CXX=${CLANGXX_TOOL} CXXFLAGS=${CLANGXX_SANITIZER_FLAGS} ${CMAKE_COMMAND}
    -DCMAKE_BUILD_TYPE=Debug -GNinja -DFK_YAML_BUILD_TEST=ON
    -S${PROJECT_SOURCE_DIR} -B${PROJECT_BINARY_DIR}/build_clang++_sanitizer
  COMMAND ${CMAKE_COMMAND} --build ${PROJECT_BINARY_DIR}/build_clang++_sanitizer --config Debug
  COMMAND cd ${PROJECT_BINARY_DIR}/build_clang++_sanitizer && ${CMAKE_CTEST_COMMAND} -C Debug --output-on-failure
  COMMENT "Compile and test with clang++ & sanitizers"
)
