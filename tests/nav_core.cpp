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

TEST_CASE("Iterate over values", "[core]") {
    auto values = nav::enum_values_v<RainbowColors>;

    RainbowColors reference_values[] {
        RainbowColors::Red,
        RainbowColors::Orange,
        RainbowColors::Yellow,
        RainbowColors::Green,
        RainbowColors::Blue,
        RainbowColors::Indigo,
        RainbowColors::Violet};

    size_t index = 0;
    for(RainbowColors value : values) {
        REQUIRE(value == reference_values[index]);
        index++;
    }
}


TEST_CASE("Iterate over names", "[core]") {
    auto names = nav::enum_names_v<RainbowColors>;

    std::string_view reference_names[] {
        "Red",
        "Orange",
        "Yellow",
        "Green",
        "Blue",
        "Indigo",
        "Violet"};

    size_t index = 0;
    for(std::string_view name : names) {
        REQUIRE(name == reference_names[index++]);
    }
}
