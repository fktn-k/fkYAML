add_subdirectory(unit_test)

if(FK_YAML_USE_YAML_TEST_SUITE)
  add_subdirectory(yaml_test_suite_runner)
endif()

if(FK_YAML_BUILD_ALL_TEST)
  add_subdirectory(cmake_add_subdirectory_test)
  add_subdirectory(cmake_fetch_content_test)
  add_subdirectory(cmake_find_package_test)
  add_subdirectory(cmake_target_include_directories_test)
endif()
