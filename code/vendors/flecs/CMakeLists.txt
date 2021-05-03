file(GLOB SRCS *.h
	flecs.c
	flecs_json.c
	flecs_meta.c
	flecs_rest.c
	flecs_player.c
	flecs_rest.c
	flecs_systems_civetweb.c
	flecs_dash.c
	flecs_monitor.c
	flecs-os_api-stdcpp.cpp
	flecs_components_http.c
)
add_library(flecs-bundle STATIC ${SRCS})
