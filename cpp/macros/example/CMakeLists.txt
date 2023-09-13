add_library(update_control_hsm_lib
        UpdateControlHsm.cpp
)
target_link_libraries(update_control_hsm_lib
        eta_hsm_lib
)