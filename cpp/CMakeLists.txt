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

find_package(GTest)
if(GTest_FOUND)
    message("Found previously installed googletest library")
else()
    message("Did not find previously installed googletest library, will download...")
    FetchContent_Declare(googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.14.0
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()


# wise_enum doesn't seem to provide a config.cmake file, but since it is header only,
# we can probably get away with just adding it to the include path
# find_package(wise_enum REQUIRED)
include_directories(/usr/local/include/wise_enum)

if(EXISTS "/usr/local/include/wise_enum/wise_enum.h")
    message("Found previously installed wise_enum library")
    include_directories(/usr/local/include/wise_enum)
else()
    message("Did not find previously installed wise_enum library, will download...")
    FetchContent_Declare(WiseEnum
            GIT_REPOSITORY https://github.com/quicknir/wise_enum.git
            GIT_TAG master
    )
    FetchContent_MakeAvailable(WiseEnum)
    # TODO: revise wise_enum include solution
    # Note: fetching as 'wise_enum' allows including using 'wise_enum/wise_enum.h'
    FetchContent_Populate(wise-enum-include
            GIT_REPOSITORY https://github.com/quicknir/wise_enum.git
            SOURCE_DIR "${FETCHCONTENT_BASE_DIR}/wise_enum"
    )
    include_directories(SYSTEM ${FETCHCONTENT_BASE_DIR})
endif()

add_library(eta_hsm_lib
    Hsm.cpp
    AutoLoggedStateMachine.cpp
)

add_subdirectory(examples)
add_subdirectory(tests)
add_subdirectory(utils)
