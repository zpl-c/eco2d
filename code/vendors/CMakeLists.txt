add_subdirectory(flecs)
add_subdirectory(cwpack)
add_subdirectory(raylib-nuklear)

add_library(vendors-bundle STATIC
	sfd.c
  3rd_sqlite3.c
)

