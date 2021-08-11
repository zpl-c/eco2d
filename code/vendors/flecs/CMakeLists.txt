use_cxx11()

file(GLOB SRCS *.h
	flecs.c
	flecs_meta.c

	flecs-os_api-stdcpp.cpp
)

add_library(flecs-bundle STATIC ${SRCS})
