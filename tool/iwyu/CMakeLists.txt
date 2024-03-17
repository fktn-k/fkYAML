###############################
#   Prepare dependent tools   #
###############################

# find include-what-you-use.
find_program(IWYU_TOOL NAMES include-what-you-use iwyu REQUIRED)
execute_process(
  COMMAND ${IWYU_TOOL} --version
  OUTPUT_VARIABLE IWYU_TOOL_VERSION
  ERROR_VARIABLE IWYU_TOOL_VERSION
)
string(REGEX MATCH "[0-9]+(\\.[0-9]+)+" IWYU_TOOL_VERSION "${IWYU_TOOL_VERSION}")
message(STATUS "Found iwyu. version: ${IWYU_TOOL_VERSION} path: ${IWYU_TOOL}")

###############################
#   Configure build targets   #
###############################

add_custom_target(run_iwyu COMMENT "Check if headers are self-contained.")

file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/include/fkYAML/*.hpp)
set(IWYU_PATH_AND_OPTIONS ${IWYU_TOOL} -Xiwyu --max_line_length=300 -Xiwyu --comments)

foreach(SRC_FILE ${SRC_FILES})
  file(RELATIVE_PATH REL_SRC_FILE "${PROJECT_SOURCE_DIR}/include/fkYAML" "${SRC_FILE}")
  string(REPLACE "/"    "_" REL_SRC_FILE_REPLACED "${REL_SRC_FILE}")
  string(REPLACE ".hpp" ""  REL_SRC_FILE_REPLACED "${REL_SRC_FILE_REPLACED}")
  set(IWYU_TARGET_NAME iwyu_for_${REL_SRC_FILE_REPLACED})

  set(IWYU_HELPER_FILE "${CMAKE_CURRENT_BINARY_DIR}/iwyu_helper_${REL_SRC_FILE_REPLACED}.cpp")
  file(WRITE  ${IWYU_HELPER_FILE} "#include <fkYAML/${REL_SRC_FILE}> // IWYU pragma: keep\n")
  file(APPEND ${IWYU_HELPER_FILE} "int main() { return 0; }\n")

  add_executable(
    ${IWYU_TARGET_NAME}
    ${IWYU_HELPER_FILE}
  )
  target_include_directories(
    ${IWYU_TARGET_NAME}
    PRIVATE
      ${FK_YAML_INCLUDE_BUILD_DIR}
  )
  target_compile_features(
    ${IWYU_TARGET_NAME}
    PRIVATE
      cxx_std_11
  )
  set_target_properties(
    ${IWYU_TARGET_NAME}
    PROPERTIES
      CXX_INCLUDE_WHAT_YOU_USE "${IWYU_PATH_AND_OPTIONS}"
  )

  add_dependencies(run_iwyu ${IWYU_TARGET_NAME})
endforeach()

add_dependencies(${FK_YAML_TARGET_NAME} run_iwyu)
