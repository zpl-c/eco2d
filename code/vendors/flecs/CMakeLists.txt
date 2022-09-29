use_cxx11()

file(GLOB SRCS *.h
	flecs.c

	flecs-os_api-stdcpp.cpp
)

add_library(flecs-bundle STATIC ${SRCS})

if (NOT WIN32)
  target_compile_options(flecs-bundle PRIVATE "-Wno-enum-constexpr-conversion")
endif()
