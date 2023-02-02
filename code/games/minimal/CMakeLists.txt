add_executable(minimal
    src/main.c
    src/platform.c
    src/worldgen.c
    src/texgen.c
    src/rules.c
	src/game.c
)

target_compile_definitions(minimal PRIVATE CLIENT)
include_directories(src ../../foundation/src ../../../art/gen)
target_link_libraries(minimal eco2d-foundation)

link_system_libs(minimal)
