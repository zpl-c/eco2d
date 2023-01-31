add_library(ferox STATIC
	src/broadphase.c
	src/collision.c
	src/debug.c
	src/dynamics.c
	src/geometry.c
	src/timer.c
	src/utils.c
	src/world.c
)

target_include_directories(ferox PUBLIC include)
target_include_directories(ferox PRIVATE src/external)

target_link_libraries(ferox PRIVATE raylib)