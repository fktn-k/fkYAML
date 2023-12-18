cmake_minimum_required(VERSION 3.8)

#############################
#   Common configurations   #
#############################

add_library(example_common_config INTERFACE)
target_include_directories(example_common_config INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/include)
target_link_libraries(example_common_config INTERFACE ${FK_YAML_TARGET_NAME})
# target_compile_features(example_common_config INTERFACE cxx_std_11)
# set_target_properties(example_common_config PROPERTIES CXX_EXTENTIONS OFF)

# # Configure compile options in common
# target_compile_options(example_common_config INTERFACE
#   # MSVC
#   $<$<CXX_COMPILER_ID:MSVC>:
#     /W4 /EHsc /utf-8 /permissive-
#     $<$<CONFIG:Debug>:/Z7>
#     $<$<CONFIG:Release>:/Od>
#   >

#   # GNU
#   $<$<CXX_COMPILER_ID:GNU>:
#     -Wall -Wextra -pedantic -Wpedantic --all-warnings --extra-warnings
#     -Wc++0x-compat
#     -Wno-self-move # necessary to build iterator class test.
#   >

#   # Clang
#   $<$<CXX_COMPILER_ID:Clang>:
#     -Wall -pedantic -Wno-c++98-compat -Wno-c++98-compat-pedantic
#     -Wno-self-assign-overloaded -Wno-self-move # necessary to build iterator class test.
#   >
# )

###############################
#   Build tutorial projects   #
###############################

file(GLOB_RECURSE TUT_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/tutorial_*.cpp)
foreach(TUT_SRC_FILE ${TUT_SRC_FILES})
  file(RELATIVE_PATH REL_TUT_SRC_FILE ${CMAKE_CURRENT_SOURCE_DIR} ${TUT_SRC_FILE})
  string(REPLACE ".cpp" "" TUT_SRC_FILE_BASE ${REL_TUT_SRC_FILE})
  add_executable(${TUT_SRC_FILE_BASE} ${TUT_SRC_FILE})
  target_link_libraries(${TUT_SRC_FILE_BASE} ${FK_YAML_TARGET_NAME})

  add_custom_command(TARGET ${TUT_SRC_FILE_BASE} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/example.yaml $<TARGET_FILE_DIR:${TUT_SRC_FILE_BASE}>
    COMMAND $<TARGET_FILE:${TUT_SRC_FILE_BASE}> > ${CMAKE_CURRENT_SOURCE_DIR}/${TUT_SRC_FILE_BASE}.output)
endforeach()

###########################
#   Build example codes   #
###########################

execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/input.yaml )

file(GLOB_RECURSE EX_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/ex_*.cpp)
foreach(EX_SRC_FILE ${EX_SRC_FILES})
  file(RELATIVE_PATH REL_EX_SRC_FILE ${CMAKE_CURRENT_SOURCE_DIR} ${EX_SRC_FILE})
  string(REPLACE ".cpp" "" EX_SRC_FILE_BASE ${REL_EX_SRC_FILE})
  add_executable(${EX_SRC_FILE_BASE} ${EX_SRC_FILE})
  target_link_libraries(${EX_SRC_FILE_BASE} ${FK_YAML_TARGET_NAME})

  add_custom_command(TARGET ${EX_SRC_FILE_BASE} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/input.yaml $<TARGET_FILE_DIR:${EX_SRC_FILE_BASE}>
    COMMAND $<TARGET_FILE:${EX_SRC_FILE_BASE}> > ${CMAKE_CURRENT_SOURCE_DIR}/${EX_SRC_FILE_BASE}.output)
endforeach()
