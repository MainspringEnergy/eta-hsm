enable_testing()
include(GoogleTest)

add_executable(canonical_test
        canonical_test.cpp
)
target_link_libraries(canonical_test
    canonical_lib
    GTest::gtest_main
)
gtest_discover_tests(canonical_test)
