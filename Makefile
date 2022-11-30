
compile: cmake
	cd build && make

cmake: CMakeLists.txt
	@mkdir -p build
	@cd build && cmake ..
