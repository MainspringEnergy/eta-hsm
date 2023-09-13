enable_testing()

add_executable(event_bucket_test
        event_bucket_test.cpp
)
target_link_libraries(event_bucket_test
        GTest::gtest_main
)

add_executable(time_tracker_test
        time_tracker_test.cpp
)
target_link_libraries(time_tracker_test
        GTest::gtest_main
)

add_executable(timer_test
        timer_test.cpp
)
target_link_libraries(timer_test
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(event_bucket_test time_tracker_test timer_test)