add_executable(eco2d
    src/main.c
    src/platform.c
    src/worldgen.c
    src/texgen.c
    src/rules.c
)

target_compile_definitions(eco2d PRIVATE CLIENT)
include_directories(src ../../foundation/src ../../../art/gen)
target_link_libraries(eco2d eco2d-foundation)

link_system_libs(eco2d)
