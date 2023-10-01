add_library(canonical_lib
        Canonical.cpp
)
target_link_libraries(canonical_lib
        eta_hsm_lib
)

add_subdirectory(tests)