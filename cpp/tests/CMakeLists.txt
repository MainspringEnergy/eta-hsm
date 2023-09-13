# eta/hsm/tests/CMakeLists.txt
# hsm_tests is a Catch2 test suite
# Default to C++17

#eta_test(hsm
#    SOURCE
#    Canonical.cpp
#    hsm_test.cpp
#
#    DEPENDS
#    Catch
#    hsm_lib
#    )

#add_library(eta_hsm_example_control_lib
#    ExampleControl.cpp
#    )
#
#target_link_libraries(eta_hsm_example_control_lib
#    hsm_lib
#    )

#eta_test(hsm_example_control
#    SOURCE
#    hsm_example_control_test.cpp
#
#    DEPENDS
#    Catch
#    hsm_example_control_lib
#    hsm_lib
#    )

#add_library(update_control_hsm_lib
#    UpdateControlHsm.cpp
#    )
#
#target_link_libraries(update_control_hsm_lib
#    hsm_lib
#    )

#eta_test(update_control_hsm
#    SOURCE
#    update_control_hsm_test.cpp
#
#    DEPENDS
#    Catch
#    update_control_hsm_lib
#    )

add_executable(eta_hsm_cd_player
    CDPlayer.cpp
)

target_link_libraries(eta_hsm_cd_player
    eta_hsm_lib
)

add_library(eta_hsm_canonical_lib
    Canonical.cpp
)

target_link_libraries(eta_hsm_canonical_lib
    eta_hsm_lib
)

# ================== GTest ===================
enable_testing()

add_executable(canonical_test
    canonical_test.cpp
)

target_link_libraries(canonical_test
    eta_hsm_canonical_lib
    GTest::gtest_main
)

add_executable(hello_test
    hello_test.cpp
)
target_link_libraries(hello_test
    GTest::gtest_main
)


include(GoogleTest)
gtest_discover_tests(canonical_test hello_test)