#include <algorithm>
#include <cassert>
#include <catch2/catch_test_macros.hpp>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <nav/nav_charconv.hpp>
#include <random>
#include <vector>

std::string_view flp_test(
    nav::view<std::string_view> prefixes,
    std::string_view string) {
    auto* result = nav::impl::find_longest_prefix(
        prefixes,
        string,
        [](std::string_view sv) { return sv; });
    if (result) {
        return *result;
    } else {
        return {};
    }
}
std::string_view flp_reference(
    nav::view<std::string_view> prefixes,
    std::string_view string) {
    std::string_view best_prefix {};
    for (auto prefix : prefixes) {
        if (prefix.size() > best_prefix.size()
            && string.substr(0, prefix.size()) == prefix) {
            best_prefix = prefix;
        }
    }
    return best_prefix;
}

template <class RNG>
std::vector<std::string_view> split_random(
    std::string_view source,
    RNG&& rng,
    int min_len,
    int max_len) {
    std::vector<std::string_view> segments;

    std::uniform_int_distribution<> dist(min_len, max_len);

    while (source.size() > max_len) {
        int len = dist(rng);
        segments.push_back(source.substr(0, len));
        size_t old_len = source.size();
        source.remove_prefix(len);
        assert(source.size() < old_len);
    }
    return segments;
}
template <class RNG>
std::string random_string(size_t length, RNG&& rng, char min, char max) {
    std::string s(length, '\0');
    std::uniform_int_distribution<char> dist(min, max);

    for (size_t i = 0; i < length; i++) {
        s[i] = dist(rng);
    }

    return s;
}
TEST_CASE("Test find_longest_prefix", "[charconv]") {
    std::mt19937_64 gen;
    std::string source = random_string(1000, gen, 'a', 'e');
    fmt::print("Created source\n");
    auto prefixes = split_random(source, gen, 3, 8);
    fmt::print("Split random\n");

    std::sort(prefixes.begin(), prefixes.end());
    auto prefix_view = nav::view {prefixes.data(), prefixes.size()};

    for (int i = 0; i < 20; i++) {
        std::string search_string = random_string(30, gen, 'a', 'e');

        INFO(fmt::format("Search string = {}", search_string));
        INFO(fmt::format("{}", prefix_view));
        REQUIRE(
            flp_test(prefix_view, search_string)
            == flp_reference(prefix_view, search_string));
    }
}
