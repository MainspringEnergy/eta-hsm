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
include(GenerateExportHeader)
include(CMakePackageConfigHelpers)

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

# TODO: replace shenanigans below with find_package()
# find_package(WiseEnum REQUIRED)
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

add_library(eta_hsm
    Hsm.cpp
    AutoLoggedStateMachine.cpp
)
generate_export_header(eta_hsm)
install(TARGETS eta_hsm  EXPORT EtaHsmTargets
        DESTINATION lib)
install(
    FILES
        Hsm.hpp
        Hsm-inl.hpp
        AutoLoggedStateMachine.hpp
    DESTINATION include/eta_hsm
)

write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/EtaHsmConfigVersion.cmake"
        VERSION ${EtaHsm_VERSION}
        COMPATIBILITY AnyNewerVersion
)

export(EXPORT EtaHsmTargets
        FILE "${CMAKE_CURRENT_BINARY_DIR}/EtaHsmTargets.cmake"
        NAMESPACE EtaHsm::
)
configure_file(cmake/EtaHsmConfig.cmake
        "${CMAKE_CURRENT_BINARY_DIR}/EtaHsmConfig.cmake"
        COPYONLY
)

set(ConfigPackageLocation lib/cmake/EtaHsm)
install(EXPORT EtaHsmTargets
    FILE
        EtaHsmTargets.cmake
    NAMESPACE
        EtaHsm::
    DESTINATION
        ${ConfigPackageLocation}
)
install(
        FILES
        cmake/EtaHsmConfig.cmake
        "${CMAKE_CURRENT_BINARY_DIR}/EtaHsmConfigVersion.cmake"
        DESTINATION
        ${ConfigPackageLocation}
        COMPONENT
        Devel
)


add_subdirectory(examples)
add_subdirectory(tests)
add_subdirectory(utils)
