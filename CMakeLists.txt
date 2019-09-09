cmake_minimum_required(VERSION 3.7.2)

project(kainjow_mustache VERSION 4.0.0 LANGUAGES CXX)

option(MUSTACHE_BUILD_TESTS
       "build the tests with catch"
       ON)

add_library(${PROJECT_NAME}
            INTERFACE)
target_include_directories(${PROJECT_NAME}
                           INTERFACE $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/>
                                     $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

if(MUSTACHE_BUILD_TESTS)
  add_executable(${PROJECT_NAME}-test
                 mustache.hpp # to show in IDE
                 tests.cpp)
  target_compile_options(${PROJECT_NAME}-test
                         PRIVATE $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-Wall>
                                 $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-Wextra>
                                 $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-Werror>
                                 $<$<CXX_COMPILER_ID:MSVC>:/W3>
                                 $<$<CXX_COMPILER_ID:MSVC>:/WX>)
  set_property(TARGET ${PROJECT_NAME}-test
               PROPERTY CXX_STANDARD
                        11)

endif()

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}_Targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${PROJECT_SOURCE_DIR}/mustache.hpp DESTINATION include/kainjow)
install(EXPORT ${PROJECT_NAME}_Targets
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE kainjow::
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

include(CMakePackageConfigHelpers)
write_basic_package_version_file("${PROJECT_NAME}ConfigVersion.cmake"
                                 VERSION ${PROJECT_VERSION}
                                 COMPATIBILITY SameMajorVersion)
configure_package_config_file("${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
                              "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
                              INSTALL_DESTINATION
                              ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
              "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)
