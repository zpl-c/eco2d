file(GLOB_RECURSE SRCS *.h *.c)
add_library(eco2d-common STATIC ${SRCS})
