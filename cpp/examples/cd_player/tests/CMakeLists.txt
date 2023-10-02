enable_testing()
include(GoogleTest)

add_executable(cd_player_test
        cd_player_test.cpp
)
target_link_libraries(cd_player_test
        cd_player_lib
        GTest::gtest_main
)
gtest_discover_tests(cd_player_test)