############################################
#   configure C++ standard for unit test   #
############################################

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

############################################################
#   Configure common compile options for unit/fuzz tests   #
############################################################

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

#############################
#   Register test targets   #
#############################

add_subdirectory(unit_test)

if(FK_YAML_BUILD_FUZZ_TEST)
  add_subdirectory(fuzz_test)
endif()

if(FK_YAML_BUILD_ALL_TEST)
  add_subdirectory(cmake_add_subdirectory_test)
  add_subdirectory(cmake_fetch_content_test)
  add_subdirectory(cmake_find_package_test)
  add_subdirectory(cmake_target_include_directories_test)
endif()
