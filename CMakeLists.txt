cmake_minimum_required(VERSION 3.0.2)
project(mustache)

add_executable(mustache-unit-tests
	tests.cpp
    mustache
)

set_target_properties(mustache-unit-tests PROPERTIES CMAKE_CXX_STANDARD 17)

if (UNIX)
    target_compile_options(mustache-unit-tests PRIVATE -Wall -Wextra -Werror)
elseif (MSVC)
    target_compile_options(mustache-unit-tests PRIVATE /W3 /WX)
endif()

add_library(mustache INTERFACE)
target_sources(mustache INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/mustache.hpp)
target_include_directories(mustache INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include/mustache>
        )