add_executable(minimal
    src/main.c
    src/platform.c
)

target_compile_definitions(minimal PRIVATE CLIENT)
include_directories(src ../../foundation/src ../../../art/gen)
target_link_libraries(minimal eco2d-foundation)
target_compile_options(minimal PRIVATE -Werror -Wall -Wextra -Wno-unused-function -Wno-unknown-pragmas -Wno-unused-variable -Wno-unused-parameter)

link_system_libs(minimal)
