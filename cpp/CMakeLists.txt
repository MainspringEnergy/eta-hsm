cmake_minimum_required(VERSION 3.10.2)

project(EtaHsm VERSION 1.0.0 LANGUAGES CXX)

# Default to C++17
if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
endif ()

if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif ()

include(CTest)

include(FetchContent)
FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(WiseEnum
        GIT_REPOSITORY https://github.com/quicknir/wise_enum.git
        GIT_TAG master
)
FetchContent_MakeAvailable(WiseEnum)

add_library(eta_hsm_lib
    Hsm.cpp
    )

add_subdirectory(examples)
add_subdirectory(tests)
add_subdirectory(utils)
