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

set(TEST_TARGET "fkYAMLUnitTest")

add_executable(${TEST_TARGET}
  LexicalAnalyzerTest.cpp
  NodeClassTest.cpp
  main.cpp
)

target_include_directories(${TEST_TARGET} PRIVATE
  ${PROJECT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}
)

function(add_unit_test CLASS_NAME FUNC_NAME TARGET_NAME ID)
  add_test(NAME ${CLASS_NAME}_${FUNC_NAME} COMMAND $<TARGET_FILE:${TARGET_NAME}> ${ID})
endfunction()

add_unit_test(NodeClassTest DefaultCtorTest                ${TEST_TARGET} 0x0000)
add_unit_test(NodeClassTest SequenceNodeFactoryTest        ${TEST_TARGET} 0x0001)
add_unit_test(NodeClassTest MappingNodeFactoryTest         ${TEST_TARGET} 0x0002)
add_unit_test(NodeClassTest BoolNodeFactoryTest            ${TEST_TARGET} 0x0003)
add_unit_test(NodeClassTest SignedIntegerNodeFactoryTest   ${TEST_TARGET} 0x0004)
add_unit_test(NodeClassTest UnsignedIntegerNodeFactoryTest ${TEST_TARGET} 0x0005)
add_unit_test(NodeClassTest FloatNumberNodeFactoryTest     ${TEST_TARGET} 0x0006)
add_unit_test(NodeClassTest StringNodeFactoryTest          ${TEST_TARGET} 0x0007)
add_unit_test(NodeClassTest SequenceForLoopTest            ${TEST_TARGET} 0x0008)
add_unit_test(NodeClassTest ConstSequenceForLoopTest       ${TEST_TARGET} 0x0009)

add_unit_test(LexicalAnalyzerTest ScanDecimalSignedIntegerTest    ${TEST_TARGET} 0x0100)
add_unit_test(LexicalAnalyzerTest ScanUnsignedDecimalIntegerTest  ${TEST_TARGET} 0x0101)
add_unit_test(LexicalAnalyzerTest ScanOctalIntegerTest            ${TEST_TARGET} 0x0102)
add_unit_test(LexicalAnalyzerTest ScanHexadecimalIntegerTest      ${TEST_TARGET} 0x0103)
add_unit_test(LexicalAnalyzerTest ScanSignedFloatNumberTest       ${TEST_TARGET} 0x0104)
add_unit_test(LexicalAnalyzerTest ScanUnsignedFloatNumberTest     ${TEST_TARGET} 0x0105)
add_unit_test(LexicalAnalyzerTest ScanFloatNumberWithExponentTest ${TEST_TARGET} 0x0106)