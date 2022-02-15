#include <nav/nav.hpp>
#include <catch2/catch_test_macros.hpp>
#include <fmt/core.h>

enum nav_declare_enum(
    MyEnum, // Name
    int,    // Base type
    F1 = 1,
    F2 = 1,
    F3 = int(F1) + int(F2),
    F4 = int(F2) + int(F3),
    F5 = int(F3) + int(F4),
    F6 = int(F4) + int(F5),
    F7 = int(F5) + int(F6),
    F8 = int(F6) + int(F7),
    F9 = int(F7) + int(F8),
    F10 = int(F8) + int(F9),
);

TEST_CASE("Hello Tests") {
    fmt::print("names.size(): {}\n", nav::enum_traits<MyEnum>::names.size());
    using traits = nav::enum_traits<MyEnum>;

    for (auto value : traits::values) {
        fmt::print(
            "The name of MyEnum({}) is MyEnum::{}\n",
            int(value),
            traits::get_name(value));
    }
}
