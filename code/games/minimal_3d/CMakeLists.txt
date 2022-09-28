add_executable(minimal-3d
    src/main.c
    src/platform.c
    src/worldgen.c
    src/texgen.c
    src/rules.c
)

target_compile_definitions(minimal-3d PRIVATE CLIENT)
include_directories(src ../../foundation/src ../../../art/gen)
target_link_libraries(minimal-3d eco2d-foundation)
target_compile_options(minimal-3d PRIVATE -Werror -Wall -Wextra -Wno-unused-function -Wno-unknown-pragmas -Wno-unused-variable -Wno-unused-parameter)

link_system_libs(minimal-3d)
