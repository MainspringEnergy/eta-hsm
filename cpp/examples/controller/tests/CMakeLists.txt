enable_testing()
include(GoogleTest)

add_executable(example_control_test
        example_control_test.cpp
)
target_link_libraries(example_control_test
    example_control_lib
    GTest::gtest_main
)
gtest_discover_tests(example_control_test)

