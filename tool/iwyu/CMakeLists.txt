###############################
#   Prepare dependent tools   #
###############################

# find include-what-you-use.
find_program(IWYU_TOOL NAMES include-what-you-use iwyu REQUIRED)
execute_process(
  COMMAND ${IWYU_TOOL} --version
  OUTPUT_VARIABLE IWYU_TOOL_VERSION
  ERROR_VARIABLE IWYU_TOOL_VERSION)
string(REGEX MATCH "[0-9]+(\\.[0-9]+)+" IWYU_TOOL_VERSION "${IWYU_TOOL_VERSION}")
message(STATUS "# iwyu # version: ${IWYU_TOOL_VERSION} path: ${IWYU_TOOL}")

###############################
#   Configure build targets   #
###############################

add_custom_target(run_iwyu COMMENT "Check if headers are self-contained.")

file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/include/fkYAML/*.hpp)
set(IWYU_PATH_AND_OPTIONS ${IWYU_TOOL} -Xiwyu --max_line_length=300 -Xiwyu --comments)

foreach(SRC_FILE ${SRC_FILES})
  file(RELATIVE_PATH REL_SRC_FILE "${PROJECT_SOURCE_DIR}/include/fkYAML" "${SRC_FILE}")
  string(REPLACE "/" "_" REL_SRC_FILE_REPLACED "${REL_SRC_FILE}")
  string(REPLACE ".hpp" "" REL_SRC_FILE_REPLACED "${REL_SRC_FILE_REPLACED}")

  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/iwyu_helper.cpp.in
                 ${CMAKE_CURRENT_BINARY_DIR}/iwyu_helper_${REL_SRC_FILE_REPLACED}.cpp @ONLY)

  add_executable(iwyu_for_${REL_SRC_FILE_REPLACED} EXCLUDE_FROM_ALL
                 ${CMAKE_CURRENT_BINARY_DIR}/iwyu_helper_${REL_SRC_FILE_REPLACED}.cpp)
  target_include_directories(iwyu_for_${REL_SRC_FILE_REPLACED}
                             PRIVATE ${PROJECT_SOURCE_DIR}/include)
  target_compile_features(iwyu_for_${REL_SRC_FILE_REPLACED} PRIVATE cxx_std_11)
  set_property(TARGET iwyu_for_${REL_SRC_FILE_REPLACED} PROPERTY CXX_INCLUDE_WHAT_YOU_USE
                                                                 "${IWYU_PATH_AND_OPTIONS}")

  add_dependencies(run_iwyu iwyu_for_${REL_SRC_FILE_REPLACED})
endforeach()
