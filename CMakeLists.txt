cmake_minimum_required(VERSION 3.8)

#
# Project name and version.
#

project(fkYAML VERSION 0.0.0 LANGUAGES CXX)

#
# Check if fkYAML is built as a subproject or if it is the main project.
#

set(FK_YAML_IS_MAIN_PROJECT OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(FK_YAML_IS_MAIN_PROJECT ON)
endif()

#
# Build options.
#

if(FK_YAML_IS_MAIN_PROJECT OR FK_YAML_BUILD_TEST)
  set(FK_YAML_BUILD_TEST_INIT ON)
else()
  set(FK_YAML_BUILD_TEST_INIT OFF)
endif()

if(FK_YAML_CODE_COVERAGE)
  # Code coverage data depends on unit test app.
  # So force build it.
  set(FK_YAML_BUILD_TEST_INIT ON)
  set(FK_YAML_CODE_COVERAGE_INIT ON)
else()
  set(FK_YAML_CODE_COVERAGE_INIT OFF)
endif()

if(FK_YAML_RUN_DOXYGEN)
  set(FK_YAML_RUN_DOXYGEN_INIT ON)
else()
  set(FK_YAML_RUN_DOXYGEN_INIT OFF)
endif()

if(FK_YAML_RUN_CLANG_FORMAT)
  set(FK_YAML_RUN_CLANG_FORMAT_INIT ON)
else()
  if(FK_YAML_IS_MAIN_PROJECT)
    set(FK_YAML_RUN_CLANG_FORMAT_INIT ON)
  else()
    set(FK_YAML_RUN_CLANG_FORMAT_INIT OFF)
  endif()
endif()

if(FK_YAML_RUN_CLANG_TIDY)
  set(FK_YAML_RUN_CLANG_TIDY_INIT ON)
else()
  if(FK_YAML_IS_MAIN_PROJECT)
    set(FK_YAML_RUN_CLANG_TIDY_INIT ON)
  else()
    set(FK_YAML_RUN_CLANG_TIDY_INIT OFF)
  endif()
endif()

if(FK_YAML_DEFAULT_CI OR FK_YAML_CUSTOM_CI)
  # disable execution of clang-format/clang-tidy by default during CI.
  # these checks will be executed separately.
  set(FK_YAML_RUN_CLANG_FORMAT_INIT OFF)
  set(FK_YAML_RUN_CLANG_TIDY_INIT OFF)
endif()

option(FK_YAML_BuildTests     "Build the unit tests when FK_YAML_BUILD_TEST is enabled or if this library is the main target." ${FK_YAML_BUILD_TEST_INIT})
option(FK_YAML_GenerateCoverage "Build the unit tests with code coverage data if FK_YAML_CODE_COVERAGE is enabled." ${FK_YAML_CODE_COVERAGE_INIT})
option(FK_YAML_Install        "Install CMake targets during install step." ${FK_YAML_IS_MAIN_PROJECT})
option(FK_YAML_RunDoxygen     "Generate API documentation with doxygen." ${FK_YAML_RUN_DOXYGEN_INIT})
option(FK_YAML_RunClangFormat "Run clang-format when FK_YAML_RUN_CLANG_FORMAT is enabled or if this library is the main target." ${FK_YAML_RUN_CLANG_FORMAT_INIT})
option(FK_YAML_RunClangTidy   "Run clang-tidy when FK_YAML_RUN_CLANG_TIDY is enabled or if this library is the main target." ${FK_YAML_RUN_CLANG_TIDY_INIT})

if(FK_YAML_RunClangFormat OR FK_YAML_CUSTOM_CI)
  set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
endif()

if(FK_YAML_GenerateCoverage)
  find_program(LCOV_TOOL NAMES lcov REQUIRED)
  execute_process(COMMAND ${LCOV_TOOL} --version OUTPUT_VARIABLE LCOV_TOOL_VERSION ERROR_VARIABLE LCOV_TOOL_VERSION)
  string(REGEX MATCH "[0-9]+(\\.[0-9]+)+" LCOV_TOOL_VERSION "${LCOV_TOOL_VERSION}")
  message(STATUS "lcov ${LCOV_TOOL_VERSION} (${LCOV_TOOL})")
endif()

#
# Configurations.
#

include(GNUInstallDirs)

set(FK_YAML_TARGET_NAME                ${PROJECT_NAME})
set(FK_YAML_VERSION_STRING             "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
set(FK_YAML_CONFIG_INSTALL_DIR         "${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}" CACHE INTERNAL "Install directory path for config files.")
set(FK_YAML_INCLUDE_BUILD_DIR          "${CMAKE_CURRENT_SOURCE_DIR}/include/")
set(FK_YAML_INCLUDE_INSTALL_DIR        "${CMAKE_INSTALL_INCLUDEDIR}")
set(FK_YAML_TARGETS_EXPORT_NAME        "${PROJECT_NAME}Targets")
set(FK_YAML_CMAKE_CONFIG_TEMPLATE      "${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.cmake.in")
set(FK_YAML_CMAKE_CONFIG_DIR           "${CMAKE_CURRENT_BINARY_DIR}")
set(FK_YAML_CMAKE_VERSION_CONFIG_FILE  "${FK_YAML_CMAKE_CONFIG_DIR}/${PROJECT_NAME}ConfigVersion.cmake")
set(FK_YAML_CMAKE_PROJECT_CONFIG_FILE  "${FK_YAML_CMAKE_CONFIG_DIR}/${PROJECT_NAME}Config.cmake")
set(FK_YAML_CMAKE_PROJECT_TARGETS_FILE "${FK_YAML_CMAKE_CONFIG_DIR}/${PROJECT_NAME}Targets.cmake")
set(FK_YAML_PKGCONFIG_INSTALL_DIR      "${CMAKE_INSTALL_DATADIR}/pkgconfig")

#
# Create target and add include path.
#

add_library(${FK_YAML_TARGET_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${FK_YAML_TARGET_NAME} ALIAS ${FK_YAML_TARGET_NAME})
target_compile_features(${FK_YAML_TARGET_NAME} INTERFACE cxx_std_11)

target_include_directories(
  ${FK_YAML_TARGET_NAME} INTERFACE
  $<BUILD_INTERFACE:${FK_YAML_INCLUDE_BUILD_DIR}>
  $<INSTALL_INTERFACE:${FK_YAML_INCLUDE_INSTALL_DIR}>
)

# configure clang-format if enabled.
if(FK_YAML_RunClangFormat)
  set(FK_YAML_ClangFormatTargetPrefix "run_clang_format_for_")
  include(RunClangFormat)
  run_clang_format(
    ${FK_YAML_TARGET_NAME}
    "${CMAKE_CURRENT_SOURCE_DIR}/include/fkYAML/*.hpp"
  )
endif()

# Configure clang-tidy if enabled.
# **DO NOT** put the following lines before configuring clang-format for fkYAML library sources
# to make sure that correctly formatted source files will be tested by clang-tidy.
if(FK_YAML_RunClangTidy)
  add_subdirectory(clang_tidy)
  add_dependencies(ClangTidyHelper "${FK_YAML_ClangFormatTargetPrefix}${FK_YAML_TARGET_NAME}")
endif()

# Configure targets for CI.
if(FK_YAML_CUSTOM_CI)
  include(ci)
endif()

#
# Install a pkg-config file, so other tools can find this library.
#

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/pkg-config.pc.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc"
)

add_custom_target(build_library ALL DEPENDS ${FK_YAML_TARGET_NAME})

#
# Create and configure the unit test target.
#

if(FK_YAML_BuildTests)
  set(CATCH2_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/catch2")
  set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CATCH2_ROOT_DIR}/contrib")
  add_subdirectory(${CATCH2_ROOT_DIR})
  include(CTest)
  enable_testing()
  add_subdirectory(test)
endif()

#
# Generate API documentation if target == doxygen.
#
if(FK_YAML_RunDoxygen)
  set(FK_YAML_DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/doxygen")
  add_subdirectory(docs)
endif()

#
# Install header files, generate and install cmake config files for find_package()
#

include(CMakePackageConfigHelpers)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/fkYAMLConfigVersion.cmake.in"
  "${FK_YAML_CMAKE_VERSION_CONFIG_FILE}"
  @ONLY
)

configure_file(
  "${FK_YAML_CMAKE_CONFIG_TEMPLATE}"
  "${FK_YAML_CMAKE_PROJECT_CONFIG_FILE}"
  @ONLY
)

if(FK_YAML_Install)
  install(
    DIRECTORY "${FK_YAML_INCLUDE_BUILD_DIR}"
    DESTINATION include
  )
  install(
    FILES "${FK_YAML_CMAKE_PROJECT_CONFIG_FILE}" "${FK_YAML_CMAKE_VERSION_CONFIG_FILE}"
    DESTINATION "${FK_YAML_CONFIG_INSTALL_DIR}"
  )
  export(
    TARGETS ${FK_YAML_TARGET_NAME}
    NAMESPACE ${PROJECT_NAME}::
    FILE "${FK_YAML_CMAKE_PROJECT_TARGETS_FILE}"
  )
  install(
    TARGETS ${FK_YAML_TARGET_NAME}
    EXPORT ${FK_YAML_TARGETS_EXPORT_NAME}
  )
  install(
    EXPORT ${FK_YAML_TARGETS_EXPORT_NAME}
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION "${FK_YAML_CONFIG_INSTALL_DIR}"
  )
  install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc"
    DESTINATION "${FK_YAML_PKGCONFIG_INSTALL_DIR}"
  )
endif()