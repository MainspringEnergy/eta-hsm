add_library(example_control_lib
        ExampleControl.cpp
)
target_link_libraries(example_control_lib
        eta_hsm
)

add_subdirectory(tests)