enable_testing()
include(GoogleTest)

add_executable(hello_test
        hello_test.cpp
)
target_link_libraries(hello_test
        GTest::gtest_main
)
gtest_discover_tests(hello_test)

add_executable(canonical_test
        canonical_test.cpp
)
target_link_libraries(canonical_test
    canonical_lib
    GTest::gtest_main
)
gtest_discover_tests(canonical_test)

add_executable(example_control_test
        example_control_test.cpp
)
target_link_libraries(example_control_test
    example_control_lib
    GTest::gtest_main
)
gtest_discover_tests(example_control_test)

add_executable(cd_player_test
        cd_player_test.cpp
)
target_link_libraries(cd_player_test
        cd_player_lib
        GTest::gtest_main
)
gtest_discover_tests(cd_player_test)