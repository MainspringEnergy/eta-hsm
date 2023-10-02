add_library(example_control_lib
        ExampleControl.cpp
)
target_link_libraries(example_control_lib
        eta_hsm_lib
        wise_enum
)

add_subdirectory(tests)