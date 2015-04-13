cmake_minimum_required(VERSION 2.8)
project(mustache)
if (UNIX)
	# Travis only has CMake 2.8
	add_definitions(
		-Wall
		-Wextra
		-Werror
		-std=c++11
	)
endif()
add_executable(mustache
	mustache.hpp # to show in IDE
	tests.cpp
)
