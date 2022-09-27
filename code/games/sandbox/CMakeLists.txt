add_executable(eco2d
    src/main.c
)

target_compile_definitions(eco2d PRIVATE CLIENT)
include_directories(src ../../foundation/src ../../../art/gen)
target_link_libraries(eco2d eco2d-foundation)
target_compile_options(eco2d PRIVATE -Werror -Wall -Wextra -Wno-unused-function -Wno-unknown-pragmas -Wno-unused-variable -Wno-unused-parameter)

link_system_libs(eco2d)
