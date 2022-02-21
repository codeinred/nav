#include <catch2/catch_test_macros.hpp>
#include <nav/nav_iostream.hpp>
#include <sstream>

template <nav::nav_enum Enum>
std::string make_string(Enum value) {
    using nav::operator<<;
    std::stringstream ss;
    ss << value;
    return ss.str();
}

namespace test_nav {
enum class Color;
}

nav_declare_enum(test_nav::Color, int, Red, Green, Blue);

TEST_CASE("Testing nav_iostream", "[extra][stream]") {
    using test_nav::Color;

    // clang-format off
    REQUIRE(make_string(Color::Red)   == "Color::Red");
    REQUIRE(make_string(Color::Green) == "Color::Green");
    REQUIRE(make_string(Color::Blue)  == "Color::Blue");
    REQUIRE(make_string(Color(-1))    == "Color(-1)");
    // clang-format on
}
