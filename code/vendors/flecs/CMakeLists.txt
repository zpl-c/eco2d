file(GLOB SRCS *.c *.h)
add_library(flecs-bundle STATIC ${SRCS})
include_directories(flecs-bundle .)
