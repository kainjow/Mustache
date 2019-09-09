cmake_minimum_required(VERSION 3.7.2)

project(example_project VERSION 4.0.0 LANGUAGES CXX)

find_package(kainjow_mustache
             REQUIRED)

add_executable(my_executable ${CMAKE_CURRENT_LIST_DIR}/main.cpp)
target_link_libraries(my_executable
                      kainjow::mustache)
