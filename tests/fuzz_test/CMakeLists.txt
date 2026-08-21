############################################
#   Download data files for fuzz testing   #
############################################

include(FetchContent)
FetchContent_Declare(
  fuzz_test_data
  GIT_REPOSITORY https://github.com/fktn-k/fkyaml_fuzz_test_data.git
  GIT_TAG        main
)
FetchContent_MakeAvailable(fuzz_test_data)

#############################################
#   Build fuzz test executable for fkYAML   #
#############################################

add_executable(
  fuzz_test
  fuzzer_parse_yaml.cpp
)
target_link_libraries(
  fuzz_test
  PRIVATE
    common_test_config
)
if(MSVC)
  # MSVC silently ignores /fsanitize=undefined (warning D9002): there is no UBSan here, so neither
  # the recover nor the exclusion flag below has an equivalent. No optimization flag either, since
  # /O1 clashes with the /RTC1 the Debug config adds.
  target_compile_options(
    fuzz_test
    PRIVATE
      /fsanitize=address
      /fsanitize=fuzzer
      /Zi
  )
  target_link_options(
    fuzz_test
    PRIVATE
      /DEBUG
      /INCREMENTAL:NO
  )
else()
  target_compile_options(
    fuzz_test
    PRIVATE
      -fsanitize=fuzzer,address,undefined
      # UBSan checks are recoverable by default: without this they print a diagnostic and the
      # run continues, so libFuzzer never sees a crash and never saves the offending input.
      -fno-sanitize-recover=all
      # Excluded for the same reason the unit test configuration excludes it.
      -fno-sanitize=signed-integer-overflow
      -g
      -O1
  )
  target_link_options(
    fuzz_test
    PRIVATE
      -fsanitize=fuzzer,address,undefined
  )
endif()

#
#   Configure custom target to run the fuzz test executable with the corpus data
#

add_custom_command(
  TARGET fuzz_test
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${fuzz_test_data_SOURCE_DIR}/corpus
          $<TARGET_FILE_DIR:fuzz_test>/corpus
)

# The MSVC ASan runtime is a DLL that ships next to cl.exe; without it fuzz_test.exe
# cannot start at all.
if(MSVC)
  get_filename_component(FK_YAML_MSVC_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
  file(GLOB FK_YAML_ASAN_RUNTIME "${FK_YAML_MSVC_BIN_DIR}/clang_rt.asan_dynamic-*.dll")
  if(FK_YAML_ASAN_RUNTIME)
    add_custom_command(
      TARGET fuzz_test
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
              ${FK_YAML_ASAN_RUNTIME}
              $<TARGET_FILE_DIR:fuzz_test>
    )
  endif()
endif()

# On Windows libFuzzer cannot open -dict= through a forward-slash path and carries on
# without a dictionary, so this one argument needs native separators. The corpus
# directories and -artifact_prefix= work either way.
if(WIN32)
  set(FK_YAML_FUZZ_DICT "corpus\\yaml.dict")
else()
  set(FK_YAML_FUZZ_DICT "corpus/yaml.dict")
endif()

add_custom_target(
  run_fuzz_test
  COMMAND $<TARGET_FILE:fuzz_test> corpus/generated corpus/seeds -dict=${FK_YAML_FUZZ_DICT} -artifact_prefix=corpus/findings/ -max_total_time=300
  WORKING_DIRECTORY $<TARGET_FILE_DIR:fuzz_test>
  DEPENDS fuzz_test
)
