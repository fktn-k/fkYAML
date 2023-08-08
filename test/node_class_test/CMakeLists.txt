if(MSVC)
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc /utf-8")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()
set(CMAKE_CXX_STANDARD 11)

add_executable(NodeClassTest
  NodeClassTest.cpp
  main.cpp
)

target_include_directories(NodeClassTest PRIVATE
  ${PROJECT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}
)

add_test(NAME NodeClassTest_DefaultCtorTest                COMMAND $<TARGET_FILE:NodeClassTest> 0x0000)
add_test(NAME NodeClassTest_SequenceNodeFactoryTest        COMMAND $<TARGET_FILE:NodeClassTest> 0x0001)
add_test(NAME NodeClassTest_MappingNodeFactoryTest         COMMAND $<TARGET_FILE:NodeClassTest> 0x0002)
add_test(NAME NodeClassTest_BoolNodeFactoryTest            COMMAND $<TARGET_FILE:NodeClassTest> 0x0003)
add_test(NAME NodeClassTest_SignedIntegerNodeFactoryTest   COMMAND $<TARGET_FILE:NodeClassTest> 0x0004)
add_test(NAME NodeClassTest_UnsignedIntegerNodeFactoryTest COMMAND $<TARGET_FILE:NodeClassTest> 0x0005)
add_test(NAME NodeClassTest_FloatNumberNodeFactoryTest     COMMAND $<TARGET_FILE:NodeClassTest> 0x0006)
add_test(NAME NodeClassTest_StringNodeFactoryTest          COMMAND $<TARGET_FILE:NodeClassTest> 0x0007)