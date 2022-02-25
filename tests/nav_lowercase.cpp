#include <catch2/catch_test_macros.hpp>
#include <nav/nav_lowercase.hpp>
#include <fmt/core.h>

nav_declare_enum(
    RainbowColors,
    int,
    Red = 0xff0000,
    // Orange is a mix of Red and Yellow (#ffff00)
    Orange = (Red + 0xffff00) / 2 & 0xffff00,
    Yellow = 0xffff00,
    // Green is Yellow, without the Red
    Green = Yellow - Red,
    Blue = 0x0000ff,
    Indigo = 0x8A2BE2,
    // We're representing violet as Red + Blue
    Violet = Red + Blue);


TEST_CASE("Traits on lowercase", "") {
    INFO(
        "lowercase_enum_name_list<Enum> is a stateless type that provides a "
        "way to get and iterate over the names of an enumeration in lowercase");
    REQUIRE(std::is_empty_v<nav::lowercase_enum_name_list<RainbowColors>>);
    REQUIRE(std::is_trivial_v<nav::lowercase_enum_name_list<RainbowColors>>);
}

TEST_CASE("Check lowercase names", "[lowercase]") {
    auto names = nav::lowercase_enum_names<RainbowColors>;
    REQUIRE(names[0] == "red");
    REQUIRE(names[1] == "orange");
    REQUIRE(names[2] == "yellow");
    REQUIRE(names[3] == "green");
    REQUIRE(names[4] == "blue");
    REQUIRE(names[5] == "indigo");
    REQUIRE(names[6] == "violet");
}

TEST_CASE("Iterate over lowercase names", "[lowercase]") {
    auto names = nav::lowercase_enum_names<RainbowColors>;
    auto values = nav::enum_values<RainbowColors>;

    std::string_view reference_names[] {
        "red",
        "orange",
        "yellow",
        "green",
        "blue",
        "indigo",
        "violet"};

    size_t index = 0;
    for (std::string_view name : names) {
        REQUIRE(name == reference_names[index++]);
    }
}
#if NAV_ADD_NULL_TERMINATORS
TEST_CASE("Ensure lowercase names end with null terminator", "[lowercase]") {
    auto names = nav::lowercase_enum_names<RainbowColors>;

    for (std::string_view name : names) {
        char const* c_str = name.data();
        size_t len = name.size();
        INFO(fmt::format(
            "len = {},\n"
            "c_str = \"{}\"\n"
            "(Obtained from nav::lowercase_enum_names)",
            len,
            c_str));
        REQUIRE(c_str[len] == '\0');
    }
}
#endif
