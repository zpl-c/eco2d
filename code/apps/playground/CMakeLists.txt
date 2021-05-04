add_executable(playground
  source/main.c
)

include_directories(header)
target_link_libraries(playground eco2d-common cwpack)
link_system_libs(playground)
