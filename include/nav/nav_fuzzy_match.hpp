#pragma once
#include <nav/nav_core.hpp>
#include <string_view>

namespace nav {
using size_t = decltype(sizeof(0));
/**
 * @brief Compute the Levenshtein distance through MaxLength (this should be
 * used when one of the strings has a known max length, and so there's no need
 * to compare further than that. It also ensures that no memory allocations need
 * to occur.) Computes the distance as though the strings were truncated to
 * MaxLength
 *
 * @tparam MaxLength the maximum length to measure the distance out to.
 * @param strA the first string
 * @param strB the second string
 * @return int the distance
 */
template <size_t MaxLength>
constexpr auto levenshtein_distance =
    [](std::string_view strA, std::string_view strB) -> int {
    // for all i and j, dist[i,j] will hold the Levenshtein distance between
    // the first i characters of s and the first j characters of t
    int dist[MaxLength + 1][MaxLength + 1];
    size_t lenA = std::min(MaxLength, strA.size());
    size_t lenB = std::min(MaxLength, strB.size());

    // source prefixes can be transformed into empty string by
    // dropping all characters
    for (size_t ai = 0; ai <= lenA; ai++)
        dist[ai][0] = ai;

    // target prefixes can be reached from empty source prefix
    // by inserting every character
    for (size_t bi = 0; bi <= lenB; bi++)
        dist[0][bi] = bi;

    for (size_t bi = 0; bi < lenB; bi++) {
        for (size_t ai = 0; ai < lenA; ai++) {
            int substitutionCost = !(strA[ai] == strB[bi]);

            dist[ai + 1][bi + 1] = std::min(
                std::min(
                    dist[ai][bi + 1] + 1,         // deletion
                    dist[ai + 1][bi] + 1),        // insertion
                dist[ai][bi] + substitutionCost); // substitution
        }
    }
    return dist[lenA][lenB];
};

template <size_t MaxLength>
constexpr auto caseless_levenshtein_distance =
    [](std::string_view strA, std::string_view strB) -> int {
    char buffA[MaxLength];
    char buffB[MaxLength];

    size_t lenA = std::min(MaxLength, strA.size());
    size_t lenB = std::min(MaxLength, strB.size());

    for (size_t i = 0; i < lenA; i++) {
        buffA[i] = impl::to_lower(strA[i]);
    }
    for (size_t i = 0; i < lenB; i++) {
        buffB[i] = impl::to_lower(strB[i]);
    }
    return levenshtein_distance<MaxLength>(
        std::string_view(buffA, lenA),
        std::string_view(buffB, lenB));
};

/**
 * @brief Use fuzzy matching to find the index of the option closest to the
 * given value, from an array of options. If two options have the same distance,
 * takes the first one. Returns std::string_view::npos if num_options == 0
 *
 * @tparam MaxLength the maximum length to measure the levenshtein distance out
 * to.
 * @param options pointer to array of options to compare against
 * @param num_options number of options
 * @param value the value
 * @return size_t the index of the closest option
 */
template <class EditDistance>
constexpr size_t fuzzy_search(
    view<std::string_view> options,
    std::string_view value,
    EditDistance dist) {
    size_t best_i = options.npos;
    size_t best_dist = ~(size_t)0;
    for (size_t i = 0; i < options.size(); i++) {
        int current_dist = dist(options[i], value);
        if (current_dist < best_dist) {
            best_dist = current_dist;
            best_i = i;
        }
    }
    return best_i;
}

/* Return the index of the value whose name most closely matches the
 * given name. Compare names based on levenshtein distance. Index is
 * in declaration order, so if the return value is i you can get the
 * corresponding value with values[i] and the corresponding name with
 * names[i]. Converts inputs and names to lowercase by default.*/
template <class Enum>
constexpr static auto fuzzy_match_enum = [](std::string_view name,
                                            bool use_lowercase = true) {
    using traits = enum_traits<Enum>;
    constexpr size_t MaxLength = impl::bit_ceil_minus_1(
        traits::max_name_length * 2);
    char buff[MaxLength];
    if (use_lowercase) {
        size_t N = std::min(MaxLength, name.size());
        for (size_t i = 0; i < N; i++) {
            buff[i] = impl::to_lower(name[i]);
        }
        name = std::string_view(buff, N);
    }
    return fuzzy_search(
        {use_lowercase ? traits::lowercase_names.data() : traits::names.data(),
         traits::size},
        name,
        levenshtein_distance<MaxLength>);
};
} // namespace nav
