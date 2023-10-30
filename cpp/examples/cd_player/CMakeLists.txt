add_library(cd_player_lib
        CDPlayer.cpp
)
target_link_libraries(cd_player_lib
        eta_hsm
)

add_executable(cd_player
        main.cpp
)
target_link_libraries(cd_player
        cd_player_lib
)

add_subdirectory(tests)