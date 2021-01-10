add_executable(eco2d-server
  source/main.c
  source/network.c

  header/network.h
)

include_directories(header/)
target_link_libraries(eco2d-server raylib)
