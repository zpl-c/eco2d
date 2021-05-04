file(GLOB_RECURSE SRCS *.h *.c packets/*.c)
add_library(eco2d-common STATIC ${SRCS})
