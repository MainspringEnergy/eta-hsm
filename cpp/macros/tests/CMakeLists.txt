enable_testing()

add_executable(update_control_hsm_test
        update_control_hsm_test.cpp
)
target_link_libraries(update_control_hsm_test
        update_control_hsm_lib
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(update_control_hsm_test)