tests:
	mkdir -p build
	cd build && cmake ..
	cmake --build build
	./build/mustache
	
xcode:
	mkdir -p build_xcode
	cd build_xcode && cmake -GXcode ..
	open build_xcode/*.xcodeproj

clean:
	rm -rf build build_xcode
