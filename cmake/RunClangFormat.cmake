find_program(CLANG_FORMAT_EXE clang-format)

function(run_clang_format target pattern)
  if(CLANG_FORMAT_EXE)
    message(STATUS "Runs clang-format for ${target}. pattern: ${pattern}")
    message(STATUS "clang-format location: ${CLANG_FORMAT_EXE}")
    set(target_for_clang_format "run_clang_format_for_${target}")
    add_custom_target(
      ${target_for_clang_format}
      ALL
      COMMAND "${CLANG_FORMAT_EXE}" --version
      COMMAND "${CLANG_FORMAT_EXE}" ${pattern} -i -style=file
    )
  else()
    message(FATAL "Failed to find clang-format.exe")
  endif()
endfunction()
