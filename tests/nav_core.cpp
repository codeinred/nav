#include <catch2/catch_test_macros.hpp>
#include <nav/nav_core.hpp>
#include <type_traits>

nav_declare_enum(
    RainbowColors,
    int,
    Red,
    Orange,
    Yellow,
    Green,
    Blue,
    Indigo,
    Violet);

TEST_CASE("Count values", "[core]") {
    REQUIRE(nav::is_nav_enum_v<RainbowColors>);
    REQUIRE(nav::num_states_v<RainbowColors> == 7);
    REQUIRE(nav::enum_values_v<RainbowColors>.size() == 7);
    REQUIRE(nav::enum_names_v<RainbowColors>.size() == 7);
}

TEST_CASE("Traits", "[core]") {
    REQUIRE(std::is_empty_v<nav::enum_type_info<RainbowColors>>);
    REQUIRE(std::is_empty_v<nav::enum_value_list<RainbowColors>>);
    REQUIRE(std::is_empty_v<nav::enum_name_list<RainbowColors>>);
    REQUIRE(std::is_trivial_v<nav::enum_type_info<RainbowColors>>);
    REQUIRE(std::is_trivial_v<nav::enum_value_list<RainbowColors>>);
    REQUIRE(std::is_trivial_v<nav::enum_name_list<RainbowColors>>);
}

TEST_CASE("Check values", "[core]") {
    auto rainbow_values = nav::enum_values_v<RainbowColors>;
    INFO("enum_value_list<Enum> is a stateless type that provides a way to get "
         "and iterate over the values of an enumeration");
    REQUIRE(sizeof(rainbow_values) == 1);
    for (int i = 0; i < nav::num_states_v<RainbowColors>; i++) {
        REQUIRE(rainbow_values[i] == RainbowColors(i));
    }
}
TEST_CASE("Check names", "[core]") {
    auto rainbow_names = nav::enum_names_v<RainbowColors>;
    INFO("enum_name_list<Enum> is a stateless type that provides a way to get "
         "and iterate over the names of an enumeration");
    REQUIRE(sizeof(rainbow_names) == 1);
    REQUIRE(rainbow_names[0] == "Red");
    REQUIRE(rainbow_names[1] == "Orange");
    REQUIRE(rainbow_names[2] == "Yellow");
    REQUIRE(rainbow_names[3] == "Green");
    REQUIRE(rainbow_names[4] == "Blue");
    REQUIRE(rainbow_names[5] == "Indigo");
    REQUIRE(rainbow_names[6] == "Violet");
}
