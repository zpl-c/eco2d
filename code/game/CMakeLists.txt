include(FindRaylib.cmake)

populate_pkt_srcs()
add_library(client-common STATIC
    source/network.c
    source/game.c
	source/camera.c
	source/world_view.c
	source/prediction.c
  
    source/utils/options.c

    header/network.h
    ${PKT_SRCS}
)

add_executable(eco2d-client
  source/platform_raylib.c
  source/main.c
)

add_executable(eco2d-cli
  source/platform_text.c
  source/main.c
)

target_compile_definitions(client-common PRIVATE CLIENT)
set(LIBS client-common cwpack eco2d-common eco2d-modules flecs-bundle)

include_directories(header ../modules)
target_link_libraries(eco2d-client raylib ${LIBS})
target_link_libraries(eco2d-cli ${LIBS})

link_system_libs(eco2d-cli)
link_system_libs(eco2d-client)
