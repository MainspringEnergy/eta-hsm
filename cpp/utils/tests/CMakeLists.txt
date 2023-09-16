# eta/hsm/tests/CMakeLists.txt
# event_tests is a Catch2 test suite

eta_test(event
    SOURCE
    event_bucket_test.cpp
    timer_test.cpp
    criteria_checks_tests.cpp

    DEPENDS
    event_lib
    Catch
    logger_lib
    )
