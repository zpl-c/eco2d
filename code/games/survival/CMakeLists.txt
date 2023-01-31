add_executable(survival
    src/main.c
    src/platform.c
    src/worldgen.c
    src/texgen.c
    src/rules.c
)

target_compile_definitions(minimal PRIVATE CLIENT)
include_directories(src ../../foundation/src ../../../art/gen)
target_link_libraries(survival eco2d-foundation)

link_system_libs(survival)
