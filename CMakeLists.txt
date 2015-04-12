cmake_minimum_required(VERSION 3.1)
project(mustache)
add_executable(mustache
	mustache.hpp # to show in IDE
	tests.cpp
)
if (UNIX)
	target_compile_options(mustache PRIVATE
		-Wall
		-Wextra
		-Werror
	)
endif()
