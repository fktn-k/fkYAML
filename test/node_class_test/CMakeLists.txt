add_executable(NodeClassTest
  NodeClassTest.cpp
  main.cpp
)

target_include_directories(NodeClassTest PRIVATE
  ${PROJECT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}
)

add_test(NAME NodeClassTest_DefaultCtorTest        COMMAND $<TARGET_FILE:NodeClassTest> 0x0000)
add_test(NAME NodeClassTest_CtorWithBoolTest       COMMAND $<TARGET_FILE:NodeClassTest> 0x0001)