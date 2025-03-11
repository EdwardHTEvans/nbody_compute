CMAKE_ARGS=

NINJA := $(shell command -v ninja)
ifneq ($(NINJA),)
	CMAKE_ARGS+="-G Ninja"
endif

default: build
	cmake --build build

build: release

release:
	cmake -S . -B build ${CMAKE_ARGS}

debug:
	cmake -S . -B build ${CMAKE_ARGS} -DCMAKE_BUILD_TYPE=Debug

valgrind:
	valgrind --leak-check=full --show-leak-kinds=definite ./build/app/app

clean:
	rm -rf build
