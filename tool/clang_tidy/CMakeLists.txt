###############################
#   Prepare dependent tools   #
###############################

# find the clang-tidy tool.
find_program(CLANG_TIDY_EXE NAMES clang-tidy REQUIRED)
execute_process(
  COMMAND ${CLANG_TIDY_EXE} --version
  OUTPUT_VARIABLE CLANG_TIDY_VERSION
  ERROR_VARIABLE CLANG_TIDY_VERSION)
string(REGEX MATCH "[0-9]+(\\.[0-9]+)+" CLANG_TIDY_VERSION "${CLANG_TIDY_VERSION}")
message(STATUS "Found clang-tidy. location: ${CLANG_TIDY_EXE} version: ${CLANG_TIDY_VERSION}")

####################################
#   Generate a helper souce file   #
####################################

set(HELPER_FILE_PATH "${CMAKE_CURRENT_BINARY_DIR}/clang_tidy_helper.cpp")
file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/include/fkYAML/*.hpp)
foreach(SRC_FILE ${SRC_FILES})
  file(RELATIVE_PATH REL_SRC_FILE "${PROJECT_SOURCE_DIR}/include" ${SRC_FILE})
  file(APPEND ${HELPER_FILE_PATH} "#include <${REL_SRC_FILE}>\n")
endforeach()
file(APPEND ${HELPER_FILE_PATH} "\nint main() {\n    return 0;\n}")

###########################################
#   set options for the clang-tidy tool   #
###########################################

set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

####################################
#   build the helper source file   #
####################################

add_executable(ClangTidyHelper ${HELPER_FILE_PATH})
target_include_directories(ClangTidyHelper PUBLIC ${FK_YAML_INCLUDE_BUILD_DIR})
target_compile_options(ClangTidyHelper PUBLIC -O0) # no optimization

# create an alias target.
add_custom_target(run_clang_tidy DEPENDS ClangTidyHelper)

# make the library source build depends on the execution of the clang-tidy tool.
add_dependencies(${FK_YAML_TARGET_NAME} run_clang_tidy)
