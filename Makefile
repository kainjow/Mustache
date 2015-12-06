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

xcode:
	mkdir -p build_xcode
	cd build_xcode && cmake -GXcode ..
	open build_xcode/*.xcodeproj

clean:
	rm -rf mustache build build_xcode
