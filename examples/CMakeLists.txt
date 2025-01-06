#############################
#   Common configurations   #
#############################

add_library(example_common_config INTERFACE)
target_link_libraries(
  example_common_config
  INTERFACE
    ${FK_YAML_TARGET_NAME}
)
target_compile_options(
  example_common_config
  INTERFACE
    # MSVC
    $<$<CXX_COMPILER_ID:MSVC>:
      /wd4996 # examples contain deprecated APIs.
    >
    # GNU
    $<$<CXX_COMPILER_ID:GNU>:
      -Wno-deprecated-declarations # examples contain deprecated APIs.
    >
    # Clang
    $<$<CXX_COMPILER_ID:Clang>:
      -Wno-deprecated-declarations #examples contain deprecated APIs.
    >
)

###############################
#   Build tutorial projects   #
###############################

add_subdirectory(tutorials)

###########################
#   Build example codes   #
###########################

add_subdirectory(apis)
