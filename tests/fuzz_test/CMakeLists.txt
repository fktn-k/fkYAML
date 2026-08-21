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
add_custom_target(
  run_fuzz_test
  COMMAND $<TARGET_FILE:fuzz_test> corpus/generated corpus/seeds -dict=corpus/yaml.dict -artifact_prefix=corpus/findings/ -max_total_time=300
  WORKING_DIRECTORY $<TARGET_FILE_DIR:fuzz_test>
  DEPENDS fuzz_test
)
