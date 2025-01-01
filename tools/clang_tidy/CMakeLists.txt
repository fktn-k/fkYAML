###############################
#   Prepare dependent tools   #
###############################

# find the clang-tidy tool.
find_program(CLANG_TIDY_EXE NAMES clang-tidy REQUIRED)
execute_process(
  COMMAND ${CLANG_TIDY_EXE} --version
  OUTPUT_VARIABLE CLANG_TIDY_VERSION
  ERROR_VARIABLE CLANG_TIDY_VERSION
)
string(REGEX MATCH "[0-9]+(\\.[0-9]+)+" CLANG_TIDY_VERSION "${CLANG_TIDY_VERSION}")
message(STATUS "Found clang-tidy. version: ${CLANG_TIDY_VERSION} path: ${CLANG_TIDY_EXE}")

####################################
#   Generate a helper souce file   #
####################################

set(HELPER_FILE_PATH "${CMAKE_CURRENT_BINARY_DIR}/clang_tidy_helper.cpp")
# clear the contents written previously to the file.
file(WRITE ${HELPER_FILE_PATH} "")

file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/include/fkYAML/*.hpp)
foreach(SRC_FILE ${SRC_FILES})
  file(RELATIVE_PATH REL_SRC_FILE "${PROJECT_SOURCE_DIR}/include" ${SRC_FILE})
  file(APPEND ${HELPER_FILE_PATH} "#include <${REL_SRC_FILE}> // NOLINT(misc-include-cleaner)\n")
endforeach()
file(APPEND ${HELPER_FILE_PATH} "\nint main() { return 0; }\n")

###########################################
#   set options for the clang-tidy tool   #
###########################################

set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE};--header-filter=include/")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

####################################
#   build the helper source file   #
####################################

add_executable(
  clang_tidy_helper
  ${HELPER_FILE_PATH}
)
target_include_directories(
  clang_tidy_helper
  PUBLIC
    ${FK_YAML_INCLUDE_BUILD_DIR}
)
target_compile_options(
  clang_tidy_helper
  PUBLIC
    -O0 # no optimization
)

# create an alias target.
add_custom_target(run_clang_tidy DEPENDS clang_tidy_helper)

# make the library source build depend on the execution of ClangTidy.
add_dependencies(${FK_YAML_TARGET_NAME} run_clang_tidy)
