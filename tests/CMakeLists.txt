add_executable(mustache-unit-tests
	tests.cpp
)

target_link_libraries(mustache-unit-tests PRIVATE mustache)

if (UNIX)
    target_compile_options(mustache-unit-tests PRIVATE -Wall -Wextra -Werror)
elseif (MSVC)
    target_compile_options(mustache-unit-tests PRIVATE /W3 /WX)
endif()

