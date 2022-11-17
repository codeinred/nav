#include "nav/nav_fuzzy_match.hpp"
#include <catch2/catch_test_macros.hpp>
#include <fmt/core.h>
#include <nav/nav_core.hpp>
#include <random>
#include <string>

// clang-format off
nav_declare_enum_class(
    CommonWords, // Name of enum
    int,         // Base type
    // A h*ck-ton of words varying in length from 5-7 leters
    About, Search, Other, Which, Their, There, Contact, Online, First, Would,
    These, Click, Service, Price, People, State, Email, Health, World, Music,
    Should, Product, System, Policy, Number, Please, Support, Message, After,
    Video, Where, Rights, Public, Books, School, Through, Links, Review, Years,
    Order, Privacy, Items, Company, Group, Under, General, January, Reviews,
    Program, Games, Could, Great, United, Hotel, Center, Store, Travel, Report,
    Member, Details, Terms, Before, Hotels, Right, Because, Local, Those,
    Using, Results, Office, Design, Posted, Address, Within, States, Phone,
    Subject, Between, Forum, Family, Based, Black, Check, Special, Prices,
    Website, Index, Being, Women, Today, South, Project, Pages, Version,
    Section, Found, Sports, House, Related, County, Photo, Members, Power,
    While, Network, Systems, Three, Total, Place, Without, Access, Think,
    North, Current, Posts, Media, Control, Water, History, Since, Guide, Board,
    Change, White, Small, Rating, During, Return, Account, Times, Sites,
    Level, Digital, Profile, Events, Hours, Image, Title, Another, Shall,
    Class, Still, Money, Quality, Every, Listing, Content, Country, Private,
    Little, Visit, Tools, Reply, Compare, Movies, Include, College, Value,
    Article, Provide, Source, Author, Press, Learn, Around, Print, Course,
    Canada, Process, Stock, Credit, Point, Science, Sales, English, Estate,
    Select, Windows, Photos, Thread, Large, Gallery, Table, However, October,
    Market, Library, Really, Action, Start, Series, Model, Human, Second,
    Movie, Forums, March, Better, Yahoo, Going, Medical, Friend, Server, Study,
    Staff, Again, Looking, Issues, April, Never, Users, Street, Topic, Comment,
    Things, Working, Against, Person, Below, Mobile, Party, Payment, Login,
    Student, Offers, Legal, Above, Recent, Stores, Problem, Memory, Social,
    August, Quote, Story, Options, Rates, Create, Young, America, Field, Paper,
    Single, Example, Girls, Latest, Changes, Night, Texas, Poker, Status,
    Browse, Issue, Range, Seller, Court);
// clang-format on

TEST_CASE("Fuzzy mach single-character misspellings", "[find_fuzzy]") {
    using traits = nav::enum_traits<CommonWords>;

    constexpr auto distance_metric = nav::caseless_levenshtein_distance<32>;

    REQUIRE(
        traits::values[nav::fuzzy_match_enum<CommonWords>("abutt")]
        == CommonWords::About);

    enum EditKind { Insert, Delete, Substitute };
    std::mt19937_64 rng;

    std::uniform_int_distribution<char> letter_dist('a', 'z');
    std::uniform_int_distribution<> edit_dist(0, 2);
    for (size_t i = 0; i < traits::size; i++) {
        std::string_view name = traits::names[i];
        auto str = std::string(name);
        std::uniform_int_distribution<size_t> index_dist(0, name.size() - 1);

        int index = index_dist(rng);
        switch (EditKind(edit_dist(rng))) {
            case Insert:
                str.insert(str.begin() + index, letter_dist(rng));
                break;
            case Delete: str.erase(str.begin() + index); break;
            case Substitute: str[index] = letter_dist(rng); break;
        }
        size_t found_i = nav::fuzzy_match_enum<CommonWords>(str);
        INFO(fmt::format(
            "Search string: '{}'\n"
            "Original name: '{}'\n"
            "Found name:    '{}'\n"
            "Dist between search and original: {}\n"
            "Dist between search and discovered: {}\n",
            str,
            name,
            traits::names[found_i],
            distance_metric(name, str),
            distance_metric(traits::names[found_i], str)));

        REQUIRE(distance_metric(str, name) <= 1);
        REQUIRE(distance_metric(str, traits::names[found_i]) <= 1);
    }
}

TEST_CASE("Fuzzy mach two-character misspellings", "[find_fuzzy]") {
    using traits = nav::enum_traits<CommonWords>;

    constexpr auto distance_metric = nav::caseless_levenshtein_distance<32>;

    REQUIRE(
        traits::values[nav::fuzzy_match_enum<CommonWords>("abutt")]
        == CommonWords::About);

    enum EditKind { Insert, Delete, Substitute };
    std::mt19937_64 rng;

    std::uniform_int_distribution<char> letter_dist('a', 'z');
    std::uniform_int_distribution<> edit_dist(0, 2);

    using index_dist_t = std::uniform_int_distribution<size_t>;
    for (size_t i = 0; i < traits::size; i++) {
        std::string_view name = traits::names[i];
        auto str = std::string(name);
        auto index_dist = index_dist_t(0, name.size() - 1);

        int index = index_dist(rng);
        switch (EditKind(edit_dist(rng))) {
            case Insert:
                str.insert(str.begin() + index, letter_dist(rng));
                break;
            case Delete: str.erase(str.begin() + index); break;
            case Substitute: str[index] = letter_dist(rng); break;
        }
        // Update the index distribution based on the new size of the string
        index_dist.param(index_dist_t::param_type(0, str.size() - 1));
        index = index_dist(rng);
        switch (EditKind(edit_dist(rng))) {
            case Insert:
                str.insert(str.begin() + index, letter_dist(rng));
                break;
            case Delete: str.erase(str.begin() + index); break;
            case Substitute: str[index] = letter_dist(rng); break;
        }
        size_t found_i = nav::fuzzy_match_enum<CommonWords>(str);
        INFO(fmt::format(
            "Search string: '{}'\n"
            "Original name: '{}'\n"
            "Found name:    '{}'\n"
            "Dist between search and original: {}\n"
            "Dist between search and discovered: {}\n",
            str,
            name,
            traits::names[found_i],
            distance_metric(name, str),
            distance_metric(traits::names[found_i], str)));

        REQUIRE(distance_metric(str, name) <= 2);
        REQUIRE(distance_metric(str, traits::names[found_i]) <= 2);
    }
}
