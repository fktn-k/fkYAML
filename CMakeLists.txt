cmake_minimum_required(VERSION 3.8)

#
# Project name and version.
#

project(fkYAML VERSION 0.0.0 LANGUAGES CXX)

#
# Preparations depending on the specified build options.
#

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  # Configure installation of the fkYAML library if this project is the main project.
  set(FK_YAML_INSTALL ON)
endif()

if(FK_YAML_CODE_COVERAGE)
  # Generating code coverage depends on the unit test app.
  # So force build it.
  set(FK_YAML_BUILD_TEST ON)
endif()

if(FK_YAML_RUN_CLANG_FORMAT OR FK_YAML_CI)
  set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
endif()

if(FK_YAML_BUILD_TEST OR FK_YAML_BUILD_ALL_TEST)
  set(CATCH2_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/catch2")
  if(NOT EXISTS ${CATCH2_ROOT_DIR}/CMakeLists.txt)
    find_package(Git REQUIRED)
    message(STATUS "Fetching git submodule: Catch2")
    execute_process(
      COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive -- ${CATCH2_ROOT_DIR}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      RESULT_VARIABLE GIT_SUBMOD_RET
    )
    if(NOT GIT_SUBMOD_RET EQUAL "0")
      message(FATAL "Failed to fetch Catch2 as a git submodule.")
    endif()
  endif()
  set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CATCH2_ROOT_DIR}/contrib")
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
if(FK_YAML_RUN_CLANG_FORMAT)
  set(FK_YAML_ClangFormatTargetPrefix "run_clang_format_for_")
  include(RunClangFormat)
  run_clang_format(
    ${FK_YAML_TARGET_NAME}
    "${CMAKE_CURRENT_SOURCE_DIR}/include/fkYAML/*.hpp"
  )
endif()

# Configure clang-tidy if enabled.
if(FK_YAML_RUN_CLANG_TIDY)
  add_subdirectory(tool/clang_tidy)
  if(FK_YAML_RUN_CLANG_FORMAT)
    add_dependencies(ClangTidyHelper "${FK_YAML_ClangFormatTargetPrefix}${FK_YAML_TARGET_NAME}")
  endif()
endif()

# Configure include-what-you-use if enabled.
if(FK_YAML_RUN_IWYU)
  add_subdirectory(tool/iwyu)
endif()

# Configure targets for CI.
if(FK_YAML_CI)
  include(ci)
endif()

#
# Install a pkg-config file, so other tools can find this library.
#

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/pkg-config.pc.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc"
)

#
# Create and configure the unit test target.
#

if(FK_YAML_BUILD_TEST OR FK_YAML_BUILD_ALL_TEST)
  add_subdirectory(${CATCH2_ROOT_DIR})
  include(CTest)
  enable_testing()
  add_subdirectory(test)
endif()

#
# Generate API documentation if target == doxygen.
#
if(FK_YAML_RUN_DOXYGEN)
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

if(FK_YAML_INSTALL)
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