######################################
#   Prepare common dependent tools   #
######################################

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

########################################
#   configure C++ standard for tests   #
########################################

# determine C++ standard used in unit test app.
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

################################
#   prepare for using Catch2   #
################################

set(CATCH2_ROOT_DIR "${PROJECT_SOURCE_DIR}/thirdparty/Catch2")
list(APPEND CMAKE_MODULE_PATH "${CATCH2_ROOT_DIR}/contrib")
include(Catch)

#############################################
#   test_main library with shared code      #
#   to speed up build and common settings   #
#############################################

add_library(test_main OBJECT main.cpp)
target_link_libraries(test_main PUBLIC ${FK_YAML_TARGET_NAME})
target_include_directories(test_main PUBLIC "${CATCH2_ROOT_DIR}/single_include")
target_compile_features(test_main PUBLIC cxx_std_${FK_YAML_TEST_TARGET_CXX_STANDARD})
set_target_properties(test_main PROPERTIES CXX_EXTENTIONS OFF)
target_compile_definitions(test_main PUBLIC $<$<CONFIG:Release>:NDEBUG>)
# Configure compile options according to the target compiler.
target_compile_options(
  test_main
  PUBLIC
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
    >
    # Clang
    $<$<CXX_COMPILER_ID:Clang>:
      -Wall -Wextra -Werror -pedantic -Wdeprecated
      -Wno-c++98-compat -Wno-c++98-compat-pedantic
      -Wno-deprecated-declarations # for testing deprecated functions
    >
    $<$<CXX_COMPILER_ID:AppleClang>:
      -Wall -Wextra -Werror -pedantic -Wdeprecated
      -Wno-deprecated-declarations # for testing deprecated functions
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
  test_main
  PUBLIC
    $<$<CXX_COMPILER_ID:IntelLLVM>:
      $<$<CONFIG:Debug>:-O0 -g>
      $<$<CONFIG:Release>:-O2>
    >
)

# additional compile options for Clang Sanitizers.
if(FK_YAML_RUN_CLANG_SANITIZERS)
  target_compile_options(
    test_main
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
      test_main
      INTERFACE
        -fno-omit-frame-pointer
        -fsanitize=address,undefined,bounds,integer,nullability
        -fno-sanitize-recover=all
        -fno-sanitize=signed-integer-overflow,implicit-conversion
    )
  else()
    target_link_libraries(
      test_main
      INTERFACE
        -fno-omit-frame-pointer
        -fsanitize=address,undefined,bounds,integer,nullability
        -fno-sanitize-recover=all
        -fno-sanitize=signed-integer-overflow,implicit-conversion
    )
  endif()
endif()

add_dependencies(test_main ${FK_YAML_TARGET_NAME})

#######################
#   build test apps   #
#######################

if(FK_YAML_BUILD_TEST)
  add_subdirectory(unit_test)
endif()

if(FK_YAML_USE_YAML_TEST_SUITE)
  add_subdirectory(yaml_test_suite_runner)
endif()

if(FK_YAML_BUILD_ALL_TEST)
  add_subdirectory(cmake_add_subdirectory_test)
  add_subdirectory(cmake_fetch_content_test)
  add_subdirectory(cmake_find_package_test)
  add_subdirectory(cmake_target_include_directories_test)
endif()
