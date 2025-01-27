cmake_minimum_required(VERSION 3.8...3.10)

################################
#   Project name and version   #
################################

project(
  fkYAML
  VERSION 0.4.2
  LANGUAGES CXX)

#############################################################
#   Preparations depending on the specified build options   #
#############################################################

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  # Configure installation of the fkYAML library if this project is the main project.
  set(FK_YAML_INSTALL ON)
endif()

if(FK_YAML_CODE_COVERAGE OR FK_YAML_RUN_VALGRIND OR FK_YAML_RUN_CLANG_SANITIZERS)
  if(FK_YAML_RUN_VALGRIND AND FK_YAML_RUN_CLANG_SANITIZERS)
    message(FATAL_ERROR "FK_YAML_RUN_VALGRIND and FK_YAML_RUN_CLANG_SANITIZERS are mutually exclusive options.")
  endif()

  if(FK_YAML_RUN_CLANG_SANITIZERS AND NOT("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))
    message(FATAL_ERROR "Compiler must be clang when FK_YAML_RUN_CLANG_SANITIZERS is enabled.")
  endif()

  # Generating code coverage depends on the unit test app. So force build it.
  set(FK_YAML_BUILD_TEST ON)
endif()

###########################################
#   Configure variables for the library   #
###########################################

include(GNUInstallDirs)

set(FK_YAML_TARGET_NAME        ${PROJECT_NAME})
set(FK_YAML_CONFIG_INSTALL_DIR "${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}" CACHE INTERNAL "Install directory path for config files.")

if(FK_YAML_USE_SINGLE_HEADER)
  set(FK_YAML_INCLUDE_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/single_include/")
else()
  set(FK_YAML_INCLUDE_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/")
endif()
set(FK_YAML_INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}")

set(FK_YAML_TARGETS_EXPORT_NAME           "${PROJECT_NAME}Targets")
set(FK_YAML_CMAKE_CONFIG_TEMPLATE         "${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.cmake.in")
set(FK_YAML_CMAKE_VERSION_CONFIG_TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}ConfigVersion.cmake.in")
set(FK_YAML_CMAKE_CONFIG_DIR              "${CMAKE_CURRENT_BINARY_DIR}")
set(FK_YAML_CMAKE_VERSION_CONFIG_FILE     ${FK_YAML_CMAKE_CONFIG_DIR}/${PROJECT_NAME}ConfigVersion.cmake)
set(FK_YAML_CMAKE_PROJECT_CONFIG_FILE     ${FK_YAML_CMAKE_CONFIG_DIR}/${PROJECT_NAME}Config.cmake)
set(FK_YAML_CMAKE_PROJECT_TARGETS_FILE    ${FK_YAML_CMAKE_CONFIG_DIR}/${PROJECT_NAME}Targets.cmake)
set(FK_YAML_PKGCONFIG_INSTALL_DIR         "${CMAKE_INSTALL_DATADIR}/pkgconfig")

##########################################
#   Create target and add include path   #
##########################################

add_library(${FK_YAML_TARGET_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${FK_YAML_TARGET_NAME} ALIAS ${FK_YAML_TARGET_NAME})
target_compile_features(
  ${FK_YAML_TARGET_NAME}
  INTERFACE
    cxx_std_11
)

target_include_directories(
  ${FK_YAML_TARGET_NAME}
  INTERFACE
    $<BUILD_INTERFACE:${FK_YAML_INCLUDE_BUILD_DIR}>
    $<INSTALL_INTERFACE:${FK_YAML_INCLUDE_INSTALL_DIR}>
)

# add .natvis file to apply custom debug view for MSVC
if(MSVC)
  set(FK_YAML_INSTALL_NATVIS ON)
  set(FK_YAML_NATVIS_FILE ${FK_YAML_TARGET_NAME}.natvis)
  target_sources(
    ${FK_YAML_TARGET_NAME}
    INTERFACE
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${FK_YAML_NATVIS_FILE}>
      $<INSTALL_INTERFACE:${FK_YAML_NATVIS_FILE}>
)
endif()

###########################
#   Build example codes   #
###########################

if(FK_YAML_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

############################
#   Integrate with tools   #
############################

# Configure clang-tidy if enabled.
if(FK_YAML_RUN_CLANG_TIDY)
  add_subdirectory(tools/clang_tidy)
endif()

# Configure include-what-you-use if enabled.
if(FK_YAML_RUN_IWYU)
  add_subdirectory(tools/iwyu)
endif()

if(FK_YAML_RUN_BENCHMARK)
  add_subdirectory(tools/benchmark)
endif()

#################################
#   Install a pkg-config file   #
#################################

###########################
#   Build the test apps   #
###########################

if(FK_YAML_BUILD_TEST OR FK_YAML_BUILD_ALL_TEST)
  include(CTest)
  enable_testing()
  add_subdirectory(tests)

  # Set the unit test app project as the Visual Studio startup project
  # if the target compiler is some version of Microsoft Visual C++ and
  # if this project is the main project.
  if(MSVC AND CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT fkYAMLUnitTest)
  endif()
endif()

#######################
#   Install package   #
#######################

include(CMakePackageConfigHelpers)

configure_file(
  ${FK_YAML_CMAKE_CONFIG_TEMPLATE}
  ${FK_YAML_CMAKE_PROJECT_CONFIG_FILE}
  @ONLY
)

configure_file(
  ${FK_YAML_CMAKE_VERSION_CONFIG_TEMPLATE}
  ${FK_YAML_CMAKE_VERSION_CONFIG_FILE}
  @ONLY
)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/pkg-config.pc.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc"
)

if(FK_YAML_INSTALL)
  install(
    DIRECTORY ${FK_YAML_INCLUDE_BUILD_DIR}
    DESTINATION ${FK_YAML_INCLUDE_INSTALL_DIR}
  )

  install(
    FILES ${FK_YAML_CMAKE_PROJECT_CONFIG_FILE} ${FK_YAML_CMAKE_VERSION_CONFIG_FILE}
    DESTINATION ${FK_YAML_CONFIG_INSTALL_DIR}
  )

  if(FK_YAML_INSTALL_NATVIS)
    install(
      FILES ${FK_YAML_NATVIS_FILE}
      DESTINATION .
    )
  endif()

  export(
    TARGETS ${FK_YAML_TARGET_NAME}
    NAMESPACE ${PROJECT_NAME}::
    FILE ${FK_YAML_CMAKE_PROJECT_TARGETS_FILE}
  )

  install(
    TARGETS ${FK_YAML_TARGET_NAME}
    EXPORT ${FK_YAML_TARGETS_EXPORT_NAME}
  )

  install(
    EXPORT ${FK_YAML_TARGETS_EXPORT_NAME}
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${FK_YAML_CONFIG_INSTALL_DIR}
  )

  install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc"
    DESTINATION ${FK_YAML_PKGCONFIG_INSTALL_DIR}
  )
endif()
