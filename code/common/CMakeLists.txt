file(GLOB SRCS *.h *.c packets/*.h world/*.h)
set(SRCS ${SRCS}
	world/blocks.c
	world/perlin.c
	world/world.c

	world/worldgen/worldgen_test.c
)
include_directories(../modules . world)
add_library(eco2d-common STATIC ${SRCS})