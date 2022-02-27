#pragma once

#include <cstdint>
#include <string_view>

namespace nav::detail {
template <uint64_t initial_value, uint64_t mul>
constexpr auto block8_hash = [](std::string_view sv) -> unsigned {
    // A prime close to a power of 2 is chosen b/c modulo can be computed with
    // shifts and additions. The compiler does this automatically.
    constexpr uint64_t p = (uint64_t(1) << 61) - 1;
    uint64_t h = initial_value;

    char const* str = sv.data();
    size_t size = sv.size();

    for (char const* end = str + size - size % 8; str != end; str += 8) {
        uint64_t item = uint64_t(str[0])       //
                      | uint64_t(str[1]) << 8  //
                      | uint64_t(str[2]) << 16 //
                      | uint64_t(str[3]) << 24 //
                      | uint64_t(str[4]) << 32 //
                      | uint64_t(str[5]) << 40 //
                      | uint64_t(str[6]) << 48 //
                      | uint64_t(str[7]) << 56;
        h = ((h * mul) + item) % p;
    }

    // remaining bytes that didn't fit in a block of 4
    int n = size % 8;
    if (n > 0) {
        uint64_t item = 0;
        for (int i = 0; i < n; i++) {
            item |= uint64_t(str[i]) << (i * 8);
        }
        h = ((h * mul) + item) % p;
    }

    return unsigned(h);
};

constexpr auto hash_sv = block8_hash<0, 31>;
} // namespace nav::detail
