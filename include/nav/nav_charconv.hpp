#pragma once
#include <charconv>
#include <nav/nav_core.hpp>

namespace nav::detail {
template <class Entry, class Fn>
auto find_longest_prefix(view<Entry> vals, std::string_view string, Fn get_key)
    -> Entry const* {
    // TODO: Fix me
    Entry const* result = nullptr;
    size_t lower_i = 0;
    size_t upper_i = vals.size() - 1;
    while (lower_i <= upper_i) {
        size_t i = (lower_i + upper_i) / 2;
        std::string_view entry = get_key(vals[i]);
        std::string_view search_key = string.substr(0, entry.size());
        auto cmp = compare(entry, search_key);

        if (cmp < 0) {
            lower_i = i + 1;
        } else if (cmp > 0) {
            upper_i = i - 1;
        } else {
            // If the size of the entry is the same as the string size, we found
            // a perfect match. Otherwise we've obtained a lower bound for the
            // possible value. A shorter prefix should never appear after a
            // longer prefix, so this works
            if (entry.size() == string.size()) {
                return &vals[i];
            } else {
                upper_i = i - 1;
                result = &vals[i];
            }
        }
    }
    return result;
}
template <class Enum>
struct from_chars_helper {
    constexpr static view<map_entry<std::string_view, Enum>>
    sorted_names_and_values() {
        return view {enum_traits<Enum>::names_to_values.entries};
    }
};
} // namespace nav::detail
namespace nav {
template <class Enum>
std::from_chars_result from_chars(
    char const* first,
    char const* last,
    Enum& value) {
    constexpr auto get_key =
        [](map_entry<std::string_view, Enum> const& entry) {
            return entry->key;
        };
    constexpr auto names_and_values = view {
        detail::from_chars_helper<Enum>::sorted_names_and_values()};

    using traits = enum_traits<Enum>;

    map_entry<std::string_view, Enum> const* id = find_longest_prefix(
        names_and_values,
        std::string_view(first, last - first),
        get_key);
    if (id) {
        auto name = id->key;
        value = id->value;
        return std::from_chars_result {first + name.size(), std::errc {}};
    } else {
        return std::from_chars_result {first, std::errc::invalid_argument};
    }
}
} // namespace nav
