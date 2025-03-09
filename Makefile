all:
	cmake -S . -B build -G Ninja
	cmake --build build

debug:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
	cmake --build build

valgrind:
	valgrind --leak-check=full --show-leak-kinds=definite ./build/app/app

clean:
	rm -rf build
