#include <nav/nav_core.hpp>
#include <fmt/core.h>
#include <iostream>

namespace FooBar {
enum class FibonacciNumbers : int64_t;
}

enum class AnotherEnum {
    Value1,
    Value2,
    Value3,
};

nav_declare_enum(
    FooBar::FibonacciNumbers, // Name
    int64_t,                  // Base type
    F0 = 0,
    F1 = 1,
    F2 = F1 + F0,
    F3 = F2 + F1,
    F4 = F3 + F2,
    F5 = F4 + F3,
    F6 = F5 + F4,
    F7 = F6 + F5,
    F8 = F7 + F6,
    F9 = F8 + F7,
    F10 = F9 + F8,
    F11 = F10 + F9,
    F12 = F11 + F10,
    F13 = F12 + F11,
    F14 = F13 + F12,
    F15 = F14 + F13,
    F16 = F15 + F14,
    F17 = F16 + F15,
    F18 = F17 + F16,
    F19 = F18 + F17,
    F20 = F19 + F18,
    F21 = F20 + F19,
    F22 = F21 + F20,
    F23 = F22 + F21,
    F24 = F23 + F22,
    F25 = F24 + F23,
    F26 = F25 + F24,
    F27 = F26 + F25,
    F28 = F27 + F26,
    F29 = F28 + F27,
    F30 = F29 + F28,
    F31 = F30 + F29,
    F32 = F31 + F30,
    F33 = F32 + F31,
    F34 = F33 + F32,
    F35 = F34 + F33,
    F36 = F35 + F34,
    F37 = F36 + F35,
    F38 = F37 + F36,
    F39 = F38 + F37,
    F40 = F39 + F38,
    F41 = F40 + F39,
    F42 = F41 + F40,
    F43 = F42 + F41,
    F44 = F43 + F42,
    F45 = F44 + F43,
    F46 = F45 + F44,
    F47 = F46 + F45,
    F48 = F47 + F46,
    F49 = F48 + F47,
    F50 = F49 + F48,
    F51 = F50 + F49,
    F52 = F51 + F50,
    F53 = F52 + F51,
    F54 = F53 + F52,
    F55 = F54 + F53,
    F56 = F55 + F54,
    F57 = F56 + F55,
    F58 = F57 + F56,
    F59 = F58 + F57,
    F60 = F59 + F58,
    F61 = F60 + F59,
    F62 = F61 + F60,
    F63 = F62 + F61,
    F64 = F63 + F62,
    F65 = F64 + F63,
    F66 = F65 + F64,
    F67 = F66 + F65,
    F68 = F67 + F66,
    F69 = F68 + F67,
    F70 = F69 + F68,
    F71 = F70 + F69,
    F72 = F71 + F70,
    F73 = F72 + F71,
    F74 = F73 + F72,
    F75 = F74 + F73,
    F76 = F75 + F74,
    F77 = F76 + F75,
    F78 = F77 + F76,
    F79 = F78 + F77,
    F80 = F79 + F78,
    F81 = F80 + F79,
    F82 = F81 + F80,
    F83 = F82 + F81,
    F84 = F83 + F82,
    F85 = F84 + F83,
    F86 = F85 + F84,
    F87 = F86 + F85,
    F88 = F87 + F86,
    F89 = F88 + F87,
    F90 = F89 + F88,
    F91 = F90 + F89,
    F92 = F91 + F90);

std::ostream& operator<<(std::ostream& os, FooBar::FibonacciNumbers fib) {
    return os << "FibonacciNumbers(" << (int64_t)fib << ")";
}
template <class A, class B, size_t N>
std::ostream& operator<<(
    std::ostream& os,
    nav::impl::binary_map<A, B, N> const& map) {
    for (auto entry : map.get_entries()) {
        os << entry.key << ": " << entry.value << "\n";
    }
    return os;
}
template <class T, size_t N>
std::ostream& operator<<(std::ostream& os, std::array<T, N> const& arr) {
    if constexpr (N == 0) {
        os << "{}";
    } else {
        os << "{" << arr[0];
        for (size_t i = 1; i < N; i++) {
            os << ", " << arr[i];
        }
        os << "}";
    }
    return os;
}
template <class T>
std::ostream& operator<<(std::ostream& os, std::optional<T> const& value) {
    if (value) {
        return os << "Some(" << *value << ")";
    } else {
        return os << "None";
    }
    return os;
}


#include <catch2/catch_test_macros.hpp>

TEST_CASE("Check fibonacci values", "[sparse-enum][fib][values]") {
    using traits = nav::enum_traits<FooBar::FibonacciNumbers>;
    int64_t fib = 0, fib_next = 1;
    for (size_t i = 0; i <= 92; i++) {
        INFO(fmt::format("Checking value of fib({})", i));
        REQUIRE(traits::values[i] == FooBar::FibonacciNumbers(fib));
        fib = std::exchange(fib_next, fib + fib_next);
    }
}
TEST_CASE("Check fibonacci names", "[sparse-enum][fib][names]") {
    using traits = nav::enum_traits<FooBar::FibonacciNumbers>;
    INFO(traits::names);

    for (size_t i = 0; i <= 92; i++) {
        REQUIRE(traits::names[i] == fmt::format("F{}", i));
    }
}
TEST_CASE(
    "Ensure maps have been constructed properly",
    "[sparse-enum][fib][maps]") {

    using traits = nav::enum_traits<FooBar::FibonacciNumbers>;
    INFO(traits::names_to_values);
    REQUIRE(traits::names_to_values.validate_map());
}

TEST_CASE("Test name lookup", "[sparse-enum][fib][name-lookup]") {

    using traits = nav::enum_traits<FooBar::FibonacciNumbers>;

    using FooBar::FibonacciNumbers;

    for (size_t i = 0; i < traits::size; i++) {
        INFO(fmt::format(
            "i = {}, traits::names[i] = {}, traits::values[i] = {}",
            i,
            traits::names[i],
            traits::base_type(traits::values[i])));
        REQUIRE(traits::names_to_values[traits::names[i]] == traits::values[i]);
    }
}
