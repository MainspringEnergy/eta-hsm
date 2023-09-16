enable_testing()

add_executable(hello_test
        hello_test.cpp
)
target_link_libraries(hello_test
        GTest::gtest_main
)

add_executable(canonical_test
        canonical_test.cpp
)
target_link_libraries(canonical_test
    canonical_lib
    GTest::gtest_main
)

add_executable(example_control_test
        example_control_test.cpp
)
target_link_libraries(example_control_test
    example_control_lib
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(canonical_test hello_test)