file(GLOB MODULES modules/*.h source/*.c)
add_library(eco2d-modules STATIC ${MODULES})
include_directories(eco2d-modules .)
