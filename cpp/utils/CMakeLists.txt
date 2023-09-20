
add_library(utils_lib
        EventBucket.cpp
        Timer.cpp
        TimeTracker.cpp
)

target_link_libraries(utils_lib
        wise_enum
)

add_subdirectory(tests)
