function(add_sanitizer_flags ENABLED)
  if(NOT ${ENABLED})
    message(STATUS "Sanitizers deactivated.")
    return()
  endif()

  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    # Common compile/link options for sanitizers
    add_compile_options("-fno-omit-frame-pointer")
    add_link_options("-fno-omit-frame-pointer")

    # AddressSanitizer
    add_compile_options("-fsanitize=address")
    add_link_options("-fsanitize=address")

    # UndefinedBehaviorSanitizer
    add_compile_options("-fsanitize=undefined")
    add_link_options("-fsanitize=undefined")

    # option to detect out-of-bounds access
    add_compile_options("-fsanitize=bounds")
    add_link_options("-fsanitize=bounds")

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      add_compile_options("-fsanitize=integer")
      add_link_options("-fsanitize=integer")

      add_compile_options("-fsanitize=nullability")
      add_link_options("-fsanitize=nullability")

      # option to print a verbose error report and exit the program
      add_compile_options("-fno-sanitize-recover=all")
      add_link_options("-fno-sanitize-recover=all")
    endif()
  elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    message(STATUS "No sanitizers enabled for MSVC compiler&linker")
  else()
    message(ERROR "Compiler not supported for sanitizers.")
  endif()

endfunction()