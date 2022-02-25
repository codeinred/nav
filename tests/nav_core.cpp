#include <catch2/catch_test_macros.hpp>
#include <fmt/core.h>
#include <nav/nav_core.hpp>
#include <type_traits>

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

TEST_CASE("Count values", "[core]") {
    REQUIRE(nav::is_nav_enum<RainbowColors>);
    REQUIRE(nav::num_states<RainbowColors> == 7);
    REQUIRE(nav::enum_values<RainbowColors>.size() == 7);
    REQUIRE(nav::enum_names<RainbowColors>.size() == 7);
}

TEST_CASE("Traits", "[core]") {
    INFO("enum_value_list<Enum> is a stateless type that provides a way to get "
         "and iterate over the values of an enumeration");
    INFO("enum_name_list<Enum> is a stateless type that provides a way to get "
         "and iterate over the names of an enumeration");
    REQUIRE(std::is_empty_v<nav::enum_type_info<RainbowColors>>);
    REQUIRE(std::is_empty_v<nav::enum_value_list<RainbowColors>>);
    REQUIRE(std::is_empty_v<nav::enum_name_list<RainbowColors>>);
    REQUIRE(std::is_trivial_v<nav::enum_type_info<RainbowColors>>);
    REQUIRE(std::is_trivial_v<nav::enum_value_list<RainbowColors>>);
    REQUIRE(std::is_trivial_v<nav::enum_name_list<RainbowColors>>);
}

TEST_CASE("Check values", "[core]") {
    auto names = nav::enum_names<RainbowColors>;
    auto values = nav::enum_values<RainbowColors>;

    RainbowColors reference_values[] {
        RainbowColors::Red,
        RainbowColors::Orange,
        RainbowColors::Yellow,
        RainbowColors::Green,
        RainbowColors::Blue,
        RainbowColors::Indigo,
        RainbowColors::Violet};
    for (int i = 0; i < nav::num_states<RainbowColors>; i++) {
        INFO(fmt::format(
            "values[{}] == RainbowColors::{} == RainbowColors(#{:0>6x})",
            i,
            names[i],
            int(values[i])));
        REQUIRE(values[i] == reference_values[i]);
    }
}
TEST_CASE("Check names", "[core]") {
    auto names = nav::enum_names<RainbowColors>;
    REQUIRE(names[0] == "Red");
    REQUIRE(names[1] == "Orange");
    REQUIRE(names[2] == "Yellow");
    REQUIRE(names[3] == "Green");
    REQUIRE(names[4] == "Blue");
    REQUIRE(names[5] == "Indigo");
    REQUIRE(names[6] == "Violet");
}

TEST_CASE("Iterate over values", "[core]") {
    auto names = nav::enum_names<RainbowColors>;
    auto values = nav::enum_values<RainbowColors>;

    RainbowColors reference_values[] {
        RainbowColors::Red,
        RainbowColors::Orange,
        RainbowColors::Yellow,
        RainbowColors::Green,
        RainbowColors::Blue,
        RainbowColors::Indigo,
        RainbowColors::Violet};

    size_t index = 0;
    for (RainbowColors value : values) {
        INFO(fmt::format(
            "values[{}] == RainbowColors::{} == RainbowColors(#{:0>6x})",
            index,
            names[index],
            int(values[index])));
        REQUIRE(value == reference_values[index]);
        index++;
    }
}


TEST_CASE("Iterate over names", "[core]") {
    auto names = nav::enum_names<RainbowColors>;
    auto values = nav::enum_values<RainbowColors>;

    std::string_view reference_names[] {
        "Red",
        "Orange",
        "Yellow",
        "Green",
        "Blue",
        "Indigo",
        "Violet"};

    size_t index = 0;
    for (std::string_view name : names) {
        INFO(fmt::format(
            "values[{}] == RainbowColors::{} == RainbowColors(#{:0>6x})",
            index,
            names[index],
            int(values[index])));
        REQUIRE(name == reference_names[index++]);
    }
}

#if NAV_ADD_NULL_TERMINATORS
TEST_CASE("Ensure names end with null terminator", "[core]") {
    auto names = nav::enum_names<RainbowColors>;

    for (std::string_view name : names) {
        char const* c_str = name.data();
        size_t len = name.size();
        INFO(fmt::format(
            "len = {},\n"
            "c_str = \"{}\"\n"
            "(Obtained from nav::enum_names)",
            len,
            c_str));
        REQUIRE(c_str[len] == '\0');
    }
}
#endif
