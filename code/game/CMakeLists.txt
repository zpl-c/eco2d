include(FindRaylib.cmake)

populate_pkt_srcs()
add_executable(eco2d
	source/platform_raylib.c
	source/main.c

	source/network.c
    	source/game.c
	source/camera.c
	source/world_view.c
	source/prediction.c
  
	source/utils/options.c

	header/network.h
	${PKT_SRCS}
)

target_compile_definitions(eco2d PRIVATE CLIENT)
include_directories(header ../modules)
target_link_libraries(eco2d raylib cwpack eco2d-common eco2d-modules flecs-bundle)

link_system_libs(eco2d)
