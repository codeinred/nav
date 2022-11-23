#include <catch2/catch_test_macros.hpp>
#include <nav/nav_core.hpp>
#include <nav/nav_libfmt.hpp>

namespace test_nav {
enum class Color;
}

nav_declare_enum_class(test_nav::Color, int, Red, Green, Blue);

TEST_CASE("Testing nav_libfmt", "[extra][fmt]") {
    using test_nav::Color;

    // clang-format off
    REQUIRE(fmt::format("{}", Color::Red)     == "Red");
    REQUIRE(fmt::format("{}", Color::Green)   == "Green");
    REQUIRE(fmt::format("{}", Color::Blue)    == "Blue");
    REQUIRE(fmt::format("{}", Color(-1))      == "Color(-1)");

    REQUIRE(fmt::format("{:Q}", Color::Red)   == "Color::Red");
    REQUIRE(fmt::format("{:Q}", Color::Green) == "Color::Green");
    REQUIRE(fmt::format("{:Q}", Color::Blue)  == "Color::Blue");
    REQUIRE(fmt::format("{:Q}", Color(-1))    == "Color(-1)");

    REQUIRE(fmt::format("{:F}", Color::Red)   == "test_nav::Color::Red");
    REQUIRE(fmt::format("{:F}", Color::Green) == "test_nav::Color::Green");
    REQUIRE(fmt::format("{:F}", Color::Blue)  == "test_nav::Color::Blue");
    REQUIRE(fmt::format("{:F}", Color(-1))    == "test_nav::Color(-1)");


    REQUIRE(fmt::format("{:12}", Color::Red)    == "Red         ");
    REQUIRE(fmt::format("{:12}", Color::Green)  == "Green       ");
    REQUIRE(fmt::format("{:12}", Color::Blue)   == "Blue        ");
    REQUIRE(fmt::format("{:12}", Color(-1))     == "Color(-1)   ");

    REQUIRE(fmt::format("{:>12}", Color::Red)   == "         Red");
    REQUIRE(fmt::format("{:>12}", Color::Green) == "       Green");
    REQUIRE(fmt::format("{:>12}", Color::Blue)  == "        Blue");
    REQUIRE(fmt::format("{:>12}", Color(-1))    == "   Color(-1)");

    REQUIRE(fmt::format("{:^12}", Color::Red)   == "    Red     ");
    REQUIRE(fmt::format("{:^12}", Color::Green) == "   Green    ");
    REQUIRE(fmt::format("{:^12}", Color::Blue)  == "    Blue    ");
    REQUIRE(fmt::format("{:^12}", Color(-1))    == " Color(-1)  ");


    REQUIRE(fmt::format("{:Q19}", Color::Red)    == "Color::Red         ");
    REQUIRE(fmt::format("{:Q19}", Color::Green)  == "Color::Green       ");
    REQUIRE(fmt::format("{:Q19}", Color::Blue)   == "Color::Blue        ");
    REQUIRE(fmt::format("{:Q19}", Color(-1))     == "Color(-1)          ");

    REQUIRE(fmt::format("{:Q>19}", Color::Red)   == "         Color::Red");
    REQUIRE(fmt::format("{:Q>19}", Color::Green) == "       Color::Green");
    REQUIRE(fmt::format("{:Q>19}", Color::Blue)  == "        Color::Blue");
    REQUIRE(fmt::format("{:Q>19}", Color(-1))    == "          Color(-1)");

    REQUIRE(fmt::format("{:Q^19}", Color::Red)   == "    Color::Red     ");
    REQUIRE(fmt::format("{:Q^19}", Color::Green) == "   Color::Green    ");
    REQUIRE(fmt::format("{:Q^19}", Color::Blue)  == "    Color::Blue    ");
    REQUIRE(fmt::format("{:Q^19}", Color(-1))    == "     Color(-1)     ");


    REQUIRE(fmt::format("{:F29}", Color::Red)    == "test_nav::Color::Red         ");
    REQUIRE(fmt::format("{:F29}", Color::Green)  == "test_nav::Color::Green       ");
    REQUIRE(fmt::format("{:F29}", Color::Blue)   == "test_nav::Color::Blue        ");
    REQUIRE(fmt::format("{:F29}", Color(-1))     == "test_nav::Color(-1)          ");

    REQUIRE(fmt::format("{:F>29}", Color::Red)   == "         test_nav::Color::Red");
    REQUIRE(fmt::format("{:F>29}", Color::Green) == "       test_nav::Color::Green");
    REQUIRE(fmt::format("{:F>29}", Color::Blue)  == "        test_nav::Color::Blue");
    REQUIRE(fmt::format("{:F>29}", Color(-1))    == "          test_nav::Color(-1)");

    REQUIRE(fmt::format("{:F^29}", Color::Red)   == "    test_nav::Color::Red     ");
    REQUIRE(fmt::format("{:F^29}", Color::Green) == "   test_nav::Color::Green    ");
    REQUIRE(fmt::format("{:F^29}", Color::Blue)  == "    test_nav::Color::Blue    ");
    REQUIRE(fmt::format("{:F^29}", Color(-1))    == "     test_nav::Color(-1)     ");
    // clang-format on
}
