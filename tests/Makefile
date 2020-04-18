default:
	g++ -O3 -Wall -Wextra -Werror -std=c++11 -o mustache tests.cpp
	./mustache

mac:
	clang++ -O3 -Wall -Wextra -Werror -std=c++11 -stdlib=libc++ -o mustache tests.cpp
	./mustache

mac14:
	clang++ -O3 -Wall -Wextra -Werror -std=c++14 -stdlib=libc++ -o mustache14 tests.cpp
	./mustache14

clang:
	clang++ -O3 -Wall -Wextra -Werror -std=c++11 -o mustache tests.cpp

# https://gcc.gnu.org/onlinedocs/gcc/Invoking-Gcov.html
coverage:
	g++ -std=c++11 -coverage -O0 -o mustache tests.cpp
	./mustache
	gcov -l tests.cpp
# We only want coverage for mustache.hpp and tests.cpp, so delete all the other *.gcov files
	find . -type f -name 'tests.cpp*.gcov' ! -name 'tests.cpp.gcov' ! -name 'tests.cpp##mustache.hpp.gcov' -delete

xcode:
	mkdir -p build_xcode
	cd build_xcode && cmake -GXcode ..
	open build_xcode/*.xcodeproj

clean:
	rm -rf mustache mustache14 build build_xcode
	rm -rf *.gcov *.gcda *.gcno # coverage artifacts
