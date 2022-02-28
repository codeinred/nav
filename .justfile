
clean:
    rm -r build
devel:
    cmake --preset=Devel
build:
    time cmake --build build
test: build
    build/nav_core
    build/nav_core_null_terminated
