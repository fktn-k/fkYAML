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

################################
#   Set up doctest framework   #
################################

include(FetchContent)
FetchContent_Declare(
  doctest
  GIT_REPOSITORY https://github.com/doctest/doctest.git
  GIT_TAG        v2.5.3
)
FetchContent_MakeAvailable(doctest)
include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)

######################################
#   Detect available C++ standards   #
######################################

# determine C++ standard used in tests.
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

########################################
#   Configure common compile options   #
########################################

add_library(common_test_config INTERFACE)
target_link_libraries(
  common_test_config
  INTERFACE
    ${FK_YAML_TARGET_NAME}
)
target_compile_features(
  common_test_config
  INTERFACE
    cxx_std_${FK_YAML_TEST_TARGET_CXX_STANDARD}
)
set_target_properties(
  common_test_config
  PROPERTIES
    CXX_EXTENSIONS OFF
)
target_compile_definitions(
  common_test_config
  INTERFACE
    $<$<CONFIG:Release>:NDEBUG>
)
# Configure compile options according to the target compiler.
target_compile_options(
  common_test_config
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
      -Wold-style-cast # regression test for https://github.com/fktn-k/fkYAML/issues/495
    >
    # Clang
    $<$<CXX_COMPILER_ID:Clang>:
      -Wall -Wextra -Werror -pedantic -Wdeprecated
      -Wno-c++98-compat -Wno-c++98-compat-pedantic
      -Wno-deprecated-declarations # for testing deprecated functions
      -Wno-unused-comparison # for testing throwable operators
      -Wno-unused-variable # for testing against old compilers
    >
    # AppleClang
    $<$<CXX_COMPILER_ID:AppleClang>:
      -Wall -Wextra -Werror -pedantic -Wdeprecated
      -Wno-deprecated-declarations # for testing deprecated functions
      -Wno-unused-comparison # for testing throwable operators
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
  common_test_config
  INTERFACE
    $<$<CXX_COMPILER_ID:IntelLLVM>:
      $<$<CONFIG:Debug>:-O0 -g>
      $<$<CONFIG:Release>:-O2>
    >
)

# additional compile options for Clang Sanitizers.
if(FK_YAML_RUN_CLANG_SANITIZERS)
  target_compile_options(
    common_test_config
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
      common_test_config
      INTERFACE
        -fno-omit-frame-pointer
        -fsanitize=address,undefined,bounds,integer,nullability
        -fno-sanitize-recover=all
        -fno-sanitize=signed-integer-overflow,implicit-conversion
    )
  else()
    target_link_libraries(
      common_test_config
      INTERFACE
        -fno-omit-frame-pointer
        -fsanitize=address,undefined,bounds,integer,nullability
        -fno-sanitize-recover=all
        -fno-sanitize=signed-integer-overflow,implicit-conversion
    )
  endif()
endif()

# additional compile options for MSVC sanitizers.
if(FK_YAML_RUN_MSVC_SANITIZERS)
  # MSVC silently ignores /fsanitize=undefined (warning D9002): AddressSanitizer is the only
  # sanitizer available here, so the recover and exclusion flags above have no equivalent.
  target_compile_options(
    common_test_config
    INTERFACE
      /fsanitize=address
      /Zi
  )

  if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.13)
    target_link_options(
      common_test_config
      INTERFACE
        /DEBUG
        /INCREMENTAL:NO
    )
  else()
    target_link_libraries(
      common_test_config
      INTERFACE
        /DEBUG
        /INCREMENTAL:NO
    )
  endif()
endif()

# additional compile options for coverage.
if(FK_YAML_CODE_COVERAGE)
  target_compile_options(
    common_test_config
    INTERFACE
      -O0        # no optimization
      -g         # generate debug info
      --coverage # set all required flags to generate code coverage data
  )

  if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.13)
    target_link_options(
      common_test_config
      INTERFACE
        --coverage
    )
  else()
    target_link_libraries(
      common_test_config
        INTERFACE
          --coverage
    )
  endif()
endif()

add_library(
  common_test_config_with_main
  OBJECT
    main.cpp
)
target_link_libraries(
  common_test_config_with_main
  PUBLIC
    common_test_config
)
target_include_directories(
  common_test_config_with_main
  PUBLIC
    "${doctest_SOURCE_DIR}"
)

add_dependencies(common_test_config ${FK_YAML_TARGET_NAME})

#######################
#   build test apps   #
#######################

if(FK_YAML_BUILD_TEST)
  add_subdirectory(unit_test)
endif()

if(FK_YAML_USE_YAML_TEST_SUITE)
  add_subdirectory(yaml_test_suite_runner)
endif()

if(FK_YAML_BUILD_FUZZ_TEST)
  add_subdirectory(fuzz_test)
endif()

if(FK_YAML_BUILD_CMAKE_INTEGRATION_TEST)
  add_subdirectory(cmake_add_subdirectory_test)
  add_subdirectory(cmake_fetch_content_test)
  add_subdirectory(cmake_find_package_test)
  add_subdirectory(cmake_target_include_directories_test)
endif()

############################################
#   Configure custom target for coverage   #
############################################

if(FK_YAML_CODE_COVERAGE)
  file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/include/fkYAML/*.hpp)

  add_custom_target(
    generate_unit_test_coverage
    COMMAND ${CMAKE_CTEST_COMMAND} -C ${CMAKE_BUILD_TYPE} --output-on-failure -E validation_compositor -E yaml_test_suite
    COMMAND cd ${PROJECT_BINARY_DIR}/tests/unit_test/CMakeFiles/unit_test.dir
    COMMAND ${LCOV_TOOL} --directory . --capture --output-file ${PROJECT_NAME}.info --rc branch_coverage=1 --rc geninfo_unexecuted_blocks=1 --ignore-errors mismatch,mismatch
    COMMAND ${LCOV_TOOL} -e ${PROJECT_NAME}.info ${SRC_FILES} --output-file ${PROJECT_NAME}.info.filtered --rc branch_coverage=1 --ignore-errors unused,unused
    COMMAND ${PROJECT_SOURCE_DIR}/thirdparty/imapdl/filterbr.py ${PROJECT_NAME}.info.filtered > ${PROJECT_NAME}.info.filtered.noexcept
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_NAME}.info.filtered.noexcept ${PROJECT_BINARY_DIR}/coverage/fkYAML_unit_test.info
    DEPENDS unit_test
    COMMENT "Execute unit test app with code coverage."
  )

  add_custom_target(
    generate_yaml_test_suite_coverage
    # Some cases in the YAML test suite fail, so we ignore the return code of the test suite runner app for now to generate coverage data.
    COMMAND ${CMAKE_CTEST_COMMAND} -C ${CMAKE_BUILD_TYPE} --output-on-failure -R yaml_test_suite || true
    COMMAND cd ${PROJECT_BINARY_DIR}/tests/yaml_test_suite_runner/CMakeFiles/yaml_test_suite.dir
    COMMAND ${LCOV_TOOL} --directory . --capture --output-file ${PROJECT_NAME}.info --rc branch_coverage=1 --rc geninfo_unexecuted_blocks=1 --ignore-errors mismatch,mismatch
    COMMAND ${LCOV_TOOL} -e ${PROJECT_NAME}.info ${SRC_FILES} --output-file ${PROJECT_NAME}.info.filtered --rc branch_coverage=1 --ignore-errors unused,unused
    COMMAND ${PROJECT_SOURCE_DIR}/thirdparty/imapdl/filterbr.py ${PROJECT_NAME}.info.filtered > ${PROJECT_NAME}.info.filtered.noexcept
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_NAME}.info.filtered.noexcept ${PROJECT_BINARY_DIR}/coverage/fkYAML_yaml_test_suite.info
    DEPENDS yaml_test_suite
    COMMENT "Execute YAML test suite runner app with code coverage."
  )

  add_custom_target(
    generate_test_coverage
    COMMAND ${LCOV_TOOL} --add-tracefile ${PROJECT_BINARY_DIR}/coverage/fkYAML_unit_test.info --add-tracefile ${PROJECT_BINARY_DIR}/coverage/fkYAML_yaml_test_suite.info --output-file ${PROJECT_BINARY_DIR}/coverage/fkYAML.info --rc branch_coverage=1
    DEPENDS generate_unit_test_coverage generate_yaml_test_suite_coverage
    COMMENT "Generate code coverage data for unit test and YAML test suite."
  )
endif()
