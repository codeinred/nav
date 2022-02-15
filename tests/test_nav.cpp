#include <nav/nav.hpp>
#include <catch2/catch_test_macros.hpp>
#include <fmt/core.h>

enum nav_declare_enum(
    MyEnum, // Name
    int,    // Base type
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O = -15,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z, );

TEST_CASE("Hello Tests") {
    fmt::print("names.size(): {}\n", nav::enum_traits<MyEnum>::names.size());
    using traits = nav::enum_traits<MyEnum>;

    for (size_t i = 0; i < traits::count; i++) {
        fmt::print(
            "Name: '{}', Value: '{}'\n",
            traits::names[i],
            int(traits::values[i]));
    }
}
