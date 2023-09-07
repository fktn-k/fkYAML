find_program(CLANG_FORMAT_EXE clang-format REQUIRED)
execute_process(COMMAND ${CLANG_FORMAT_EXE} --version OUTPUT_VARIABLE CLANG_FORMAT_VERSION ERROR_VARIABLE CLANG_FORMAT_VERSION)
string(REGEX MATCH "[0-9]+(\\.[0-9]+)+" CLANG_FORMAT_VERSION "${CLANG_FORMAT_VERSION}")
message(STATUS "Found clang-format. location: ${CLANG_FORMAT_EXE} version: ${CLANG_FORMAT_VERSION}")

add_custom_target(run_clang_format)

function(run_clang_format target pattern)
  if(CLANG_FORMAT_EXE)
    message(STATUS "Runs clang-format for ${target}. pattern: ${pattern}")
    set(target_for_clang_format "${FK_YAML_ClangFormatTargetPrefix}${target}")
    add_custom_target(
      ${target_for_clang_format}
      COMMAND "${CLANG_FORMAT_EXE}" --version
      COMMAND "${CLANG_FORMAT_EXE}" ${pattern} -i -style=file
      WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    )
    # make sure that clang-format is executed before building target sources.
    add_dependencies(${target} ${target_for_clang_format})
    # enables running clang-format without building any applications.
    add_dependencies(run_clang_format ${target_for_clang_format})
  else()
    message(FATAL "Failed to find clang-format.exe")
  endif()
endfunction()
