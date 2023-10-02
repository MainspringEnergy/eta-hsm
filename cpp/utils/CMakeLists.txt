
add_library(eta_hsm_utils
        EventBucket.cpp
        Timer.cpp
        TimeTracker.cpp
)
generate_export_header(eta_hsm_utils)
install(TARGETS eta_hsm_utils  EXPORT EtaHsmTargets
        DESTINATION lib)
install(
        FILES
        EventBucket.hpp
        FakeClock.hpp
        TestLog.hpp
        Timer.hpp
        TimeTracker.hpp
        DESTINATION include/eta_hsm/utils
)

add_subdirectory(tests)
