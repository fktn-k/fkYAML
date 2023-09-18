find_program(CLANG_TIDY_EXE NAMES clang-tidy REQUIRED)
execute_process(COMMAND ${CLANG_TIDY_EXE} --version OUTPUT_VARIABLE CLANG_TIDY_VERSION ERROR_VARIABLE CLANG_TIDY_VERSION)
string(REGEX MATCH "[0-9]+(\\.[0-9]+)+" CLANG_TIDY_VERSION "${CLANG_TIDY_VERSION}")
message(STATUS "Found clang-tidy. location: ${CLANG_TIDY_EXE} version: ${CLANG_TIDY_VERSION}")

set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE};-header-filter=../*")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(ClangTidyHelper ClangTidyHelper.cpp)
target_include_directories(ClangTidyHelper PUBLIC ${FK_YAML_INCLUDE_BUILD_DIR})
target_compile_options(ClangTidyHelper PUBLIC -O0) # no optimization

add_custom_target(run_clang_tidy DEPENDS ClangTidyHelper)

add_dependencies(${FK_YAML_TARGET_NAME} run_clang_tidy)
