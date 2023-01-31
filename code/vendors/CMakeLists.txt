add_subdirectory(flecs)
add_subdirectory(cwpack)
add_subdirectory(raylib-nuklear)
add_subdirectory(ferox)

add_library(vendors-bundle STATIC
	sfd.c
)

