set(TEST_SOURCES
    Engine/TestDisassemblerEngine.cpp
    Rules/TestJsonRuleParser.cpp
)

add_executable(KargaScannerTests ${TEST_SOURCES})
target_link_libraries(KargaScannerTests PRIVATE KargaScannerCore gtest_main)

include(GoogleTest)
gtest_discover_tests(KargaScannerTests)
