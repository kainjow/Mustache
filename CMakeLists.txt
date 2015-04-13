cmake_minimum_required(VERSION 2.8)
project(mustache)
if (UNIX)
	add_definitions(
		-Wall
		-Wextra
		-Werror
		-std=c++11
	)
elseif (MSVC)
	add_definitions(
		/W3
		/WX
	)
endif()
add_executable(mustache
	mustache.hpp # to show in IDE
	tests.cpp
)
