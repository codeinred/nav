#include <nav/nav.hpp>
#include <catch2/catch_test_macros.hpp>
#include <fmt/core.h>

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
    F2 = int64_t(F1) + int64_t(F0),
    F3 = int64_t(F2) + int64_t(F1),
    F4 = int64_t(F3) + int64_t(F2),
    F5 = int64_t(F4) + int64_t(F3),
    F6 = int64_t(F5) + int64_t(F4),
    F7 = int64_t(F6) + int64_t(F5),
    F8 = int64_t(F7) + int64_t(F6),
    F9 = int64_t(F8) + int64_t(F7),
    F10 = int64_t(F9) + int64_t(F8),
    F11 = int64_t(F10) + int64_t(F9),
    F12 = int64_t(F11) + int64_t(F10),
    F13 = int64_t(F12) + int64_t(F11),
    F14 = int64_t(F13) + int64_t(F12),
    F15 = int64_t(F14) + int64_t(F13),
    F16 = int64_t(F15) + int64_t(F14),
    F17 = int64_t(F16) + int64_t(F15),
    F18 = int64_t(F17) + int64_t(F16),
    F19 = int64_t(F18) + int64_t(F17),
    F20 = int64_t(F19) + int64_t(F18),
    F21 = int64_t(F20) + int64_t(F19),
    F22 = int64_t(F21) + int64_t(F20),
    F23 = int64_t(F22) + int64_t(F21),
    F24 = int64_t(F23) + int64_t(F22),
    F25 = int64_t(F24) + int64_t(F23),
    F26 = int64_t(F25) + int64_t(F24),
    F27 = int64_t(F26) + int64_t(F25),
    F28 = int64_t(F27) + int64_t(F26),
    F29 = int64_t(F28) + int64_t(F27),
    F30 = int64_t(F29) + int64_t(F28),
    F31 = int64_t(F30) + int64_t(F29),
    F32 = int64_t(F31) + int64_t(F30),
    F33 = int64_t(F32) + int64_t(F31),
    F34 = int64_t(F33) + int64_t(F32),
    F35 = int64_t(F34) + int64_t(F33),
    F36 = int64_t(F35) + int64_t(F34),
    F37 = int64_t(F36) + int64_t(F35),
    F38 = int64_t(F37) + int64_t(F36),
    F39 = int64_t(F38) + int64_t(F37),
    F40 = int64_t(F39) + int64_t(F38),
    F41 = int64_t(F40) + int64_t(F39),
    F42 = int64_t(F41) + int64_t(F40),
    F43 = int64_t(F42) + int64_t(F41),
    F44 = int64_t(F43) + int64_t(F42),
    F45 = int64_t(F44) + int64_t(F43),
    F46 = int64_t(F45) + int64_t(F44),
    F47 = int64_t(F46) + int64_t(F45),
    F48 = int64_t(F47) + int64_t(F46),
    F49 = int64_t(F48) + int64_t(F47),
    F50 = int64_t(F49) + int64_t(F48),
    F51 = int64_t(F50) + int64_t(F49),
    F52 = int64_t(F51) + int64_t(F50),
    F53 = int64_t(F52) + int64_t(F51),
    F54 = int64_t(F53) + int64_t(F52),
    F55 = int64_t(F54) + int64_t(F53),
    F56 = int64_t(F55) + int64_t(F54),
    F57 = int64_t(F56) + int64_t(F55),
    F58 = int64_t(F57) + int64_t(F56),
    F59 = int64_t(F58) + int64_t(F57),
    F60 = int64_t(F59) + int64_t(F58),
    F61 = int64_t(F60) + int64_t(F59),
    F62 = int64_t(F61) + int64_t(F60),
    F63 = int64_t(F62) + int64_t(F61),
    F64 = int64_t(F63) + int64_t(F62),
    F65 = int64_t(F64) + int64_t(F63),
    F66 = int64_t(F65) + int64_t(F64),
    F67 = int64_t(F66) + int64_t(F65),
    F68 = int64_t(F67) + int64_t(F66),
    F69 = int64_t(F68) + int64_t(F67),
    F70 = int64_t(F69) + int64_t(F68),
    F71 = int64_t(F70) + int64_t(F69),
    F72 = int64_t(F71) + int64_t(F70),
    F73 = int64_t(F72) + int64_t(F71),
    F74 = int64_t(F73) + int64_t(F72),
    F75 = int64_t(F74) + int64_t(F73),
    F76 = int64_t(F75) + int64_t(F74),
    F77 = int64_t(F76) + int64_t(F75),
    F78 = int64_t(F77) + int64_t(F76),
    F79 = int64_t(F78) + int64_t(F77),
    F80 = int64_t(F79) + int64_t(F78),
    F81 = int64_t(F80) + int64_t(F79),
    F82 = int64_t(F81) + int64_t(F80),
    F83 = int64_t(F82) + int64_t(F81),
    F84 = int64_t(F83) + int64_t(F82),
    F85 = int64_t(F84) + int64_t(F83),
    F86 = int64_t(F85) + int64_t(F84),
    F87 = int64_t(F86) + int64_t(F85),
    F88 = int64_t(F87) + int64_t(F86),
    F89 = int64_t(F88) + int64_t(F87),
    F90 = int64_t(F89) + int64_t(F88),
    F91 = int64_t(F90) + int64_t(F89),
    F92 = int64_t(F91) + int64_t(F90), );

TEST_CASE("Hello Tests") {

    using traits = nav::enum_traits<FooBar::FibonacciNumbers>;

    fmt::print("Max name length: {}\n", traits::max_name_length);
    using enum FooBar::FibonacciNumbers;
    static_assert(F1 <= F2);
    for (auto value : traits::values) {
        fmt::print(
            "{0}({1}) == {0}::{2}\n",
            traits::name,
            int64_t(value),
            traits::get_name(value, "<unnamed>"));
    }
}
