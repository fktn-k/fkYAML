add_subdirectory(unit_test)

if(FK_YAML_BUILD_ALL_TEST)
  add_subdirectory(cmake_add_subdirectory_test)
  add_subdirectory(cmake_fetch_content_test)
  add_subdirectory(cmake_find_package_test)
  add_subdirectory(cmake_target_include_directories_test)
endif()