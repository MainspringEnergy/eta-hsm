# eta/hsm/tests/CMakeLists.txt
# hsm_tests is a Catch2 test suite

eta_test(hsm
    SOURCE
    Canonical.cpp
    hsm_test.cpp

    DEPENDS
    Catch
    hsm_lib
    )

add_library(hsm_example_control_lib
    ExampleControl.cpp
    )

target_link_libraries(hsm_example_control_lib
    hsm_lib
    )

eta_test(hsm_example_control
    SOURCE
    hsm_example_control_test.cpp

    DEPENDS
    Catch
    hsm_example_control_lib
    hsm_lib
    )

add_library(update_control_hsm_lib
    UpdateControlHsm.cpp
    )

target_link_libraries(update_control_hsm_lib
    hsm_lib
    )

eta_test(update_control_hsm
    SOURCE
    update_control_hsm_test.cpp

    DEPENDS
    Catch
    update_control_hsm_lib
    )

if (ETA_POSIX)
    eta_executable(etahsm_cd_player
        CDPlayer.cpp
        )

    target_link_libraries(etahsm_cd_player
        hsm_lib
        )
endif()
