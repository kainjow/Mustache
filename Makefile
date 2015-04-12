tests:
	mkdir -p build
	cd build && cmake ..
	cmake --build build
	
xcode:
	mkdir -p build
	cd build && cmake -GXcode ..
	open build/*.xcodeproj

clean:
	rm -rf build
