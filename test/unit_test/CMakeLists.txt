if(MSVC)
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc /utf-8 /permissive-")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Z7")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()
set(CMAKE_CXX_STANDARD 11)

add_executable(fkYAMLUnitTest
  LexicalAnalyzerTest.cpp
  NodeClassTest.cpp
  main.cpp
)

target_include_directories(fkYAMLUnitTest PRIVATE
  ${PROJECT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}
)

add_test(NAME NodeClassTest_DefaultCtorTest                COMMAND $<TARGET_FILE:fkYAMLUnitTest> 0x0000)
add_test(NAME NodeClassTest_SequenceNodeFactoryTest        COMMAND $<TARGET_FILE:fkYAMLUnitTest> 0x0001)
add_test(NAME NodeClassTest_MappingNodeFactoryTest         COMMAND $<TARGET_FILE:fkYAMLUnitTest> 0x0002)
add_test(NAME NodeClassTest_BoolNodeFactoryTest            COMMAND $<TARGET_FILE:fkYAMLUnitTest> 0x0003)
add_test(NAME NodeClassTest_SignedIntegerNodeFactoryTest   COMMAND $<TARGET_FILE:fkYAMLUnitTest> 0x0004)
add_test(NAME NodeClassTest_UnsignedIntegerNodeFactoryTest COMMAND $<TARGET_FILE:fkYAMLUnitTest> 0x0005)
add_test(NAME NodeClassTest_FloatNumberNodeFactoryTest     COMMAND $<TARGET_FILE:fkYAMLUnitTest> 0x0006)
add_test(NAME NodeClassTest_StringNodeFactoryTest          COMMAND $<TARGET_FILE:fkYAMLUnitTest> 0x0007)
add_test(NAME NodeClassTest_SequenceForLoopTest            COMMAND $<TARGET_FILE:fkYAMLUnitTest> 0x0008)
add_test(NAME NodeClassTest_ConstSequenceForLoopTest       COMMAND $<TARGET_FILE:fkYAMLUnitTest> 0x0009)

add_test(NAME LexicalAnalyzerTest_ScanDecimalSignedIntegerTest   COMMAND $<TARGET_FILE:fkYAMLUnitTest> 0x0100)
add_test(NAME LexicalAnalyzerTest_ScanUnsignedDecimalIntegerTest COMMAND $<TARGET_FILE:fkYAMLUnitTest> 0x0101)
add_test(NAME LexicalAnalyzerTest_ScanSignedFloatNumberTest      COMMAND $<TARGET_FILE:fkYAMLUnitTest> 0x0102)
add_test(NAME LexicalAnalyzerTest_ScanUnsignedFloatNumberTest    COMMAND $<TARGET_FILE:fkYAMLUnitTest> 0x0103)