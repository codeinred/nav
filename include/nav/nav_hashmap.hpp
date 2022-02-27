#pragma once

#include <cstdint>
#include <string_view>

namespace nav::detail {
template <uint64_t initial_value, uint64_t mul>
constexpr auto basic_sv_hash = [](std::string_view sv) -> unsigned {
    // A prime close to a power of 2 is chosen b/c modulo can be computed with
    // shifts and additions. The compiler does this automatically.
    constexpr uint64_t p = (uint64_t(1) << 61) - 1;
    uint64_t h = initial_value;
    for (char ch : sv) {
        h = ((h * mul) + ch) % p;
    }

    return unsigned(h);
};

template <uint64_t initial_value, uint64_t mul>
constexpr auto basic_block_sv_hash = [](std::string_view sv) -> unsigned {
    // A prime close to a power of 2 is chosen b/c modulo can be computed with
    // shifts and additions. The compiler does this automatically.
    constexpr uint64_t p = (uint64_t(1) << 61) - 1;
    uint64_t h = initial_value;

    char const* str = sv.data();
    size_t size = sv.size();

    for (char const* end = str + size - size % 4; str != end; str += 4) {
        unsigned item = unsigned(str[0])       //
                      | unsigned(str[1]) << 8  //
                      | unsigned(str[2]) << 16 //
                      | unsigned(str[3]) << 24;
        h = ((h * mul) + item) % p;
    }

    // remaining bytes that didn't fit in a block of 4
    int n = size % 4;
    unsigned item = 0;
    for (int i = 0; i < n; i++) {
        item |= unsigned(str[i]) << (i * 8);
    }
    h = ((h * mul) + item) % p;

    return unsigned(h);
};

constexpr auto sv_hash = basic_block_sv_hash<0, 31>;
constexpr auto block_sv_hash = basic_block_sv_hash<0, 31>;

} // namespace nav::detail
