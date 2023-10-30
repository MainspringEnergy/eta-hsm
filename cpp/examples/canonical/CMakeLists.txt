add_library(canonical_lib
        Canonical.cpp
)
target_link_libraries(canonical_lib
        eta_hsm
)

add_subdirectory(tests)