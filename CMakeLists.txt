cmake_minimum_required(VERSION 3.0.2...3.10)
project(mustache)
add_library(mustache INTERFACE)

target_compile_features(mustache INTERFACE cxx_std_17)
target_sources(mustache INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/mustache.hpp)
target_include_directories(mustache INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(tests)
