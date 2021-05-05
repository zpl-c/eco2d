populate_pkt_srcs()
add_executable(playground
    source/main.c
    source/game.c

	${PKT_SRCS}
)

include_directories(header ../modules ../common)
target_link_libraries(playground eco2d-common eco2d-modules flecs-bundle cwpack)
link_system_libs(playground)
