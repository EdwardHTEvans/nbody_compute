CMAKE_ARGS=

NINJA := $(shell command -v ninja)
ifneq ($(NINJA),)
	CMAKE_ARGS+="-G Ninja"
endif

all:
	cmake -S . -B build ${CMAKE_ARGS}
	cmake --build build

debug:
	cmake -S . -B build ${CMAKE_ARGS} -DCMAKE_BUILD_TYPE=Debug
	cmake --build build

valgrind:
	valgrind --leak-check=full --show-leak-kinds=definite ./build/app/app

clean:
	rm -rf build
