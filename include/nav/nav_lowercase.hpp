#ifndef NAV_NAV_LOWERCASE
#define NAV_NAV_LOWERCASE

// Checking for this makes it easier to use in places like godbolt where it's
// being loaded from github
#ifndef NAV_NAV_CORE
#include "nav_core.hpp"
#endif

namespace nav {
template <class Enum>
struct lowercase_enum_name_list : enum_type_info<Enum> {
    using block_type = typename enum_name_list<Enum>::block_type;

   private:
    constexpr static block_type name_block {[](block_type& dest) {
        block_type const& source = enum_name_list<Enum>::get_name_block();
        {
            size_t N = source.block_size();
            for (size_t i = 0; i < N; i++) {
                char ch = source.data[i];
                dest.data[i] = 'A' <= ch && ch <= 'Z' ? ch - 'A' + 'a' : ch;
            }
        }
        {
            size_t N = source.size();
            for (size_t i = 0; i <= N; i++) {
                dest.offsets[i] = source.offsets[i];
            }
        }
    }};

   public:
    using iterator = string_block_iterator;
    using const_iterator = iterator;
    constexpr iterator begin() const noexcept {
        return name_block.begin();
    }
    constexpr iterator end() const noexcept {
        return name_block.end();
    }
    constexpr std::string_view operator[](size_t i) const noexcept {
        auto off1 = name_block.offsets[i];
        auto off2 = name_block.offsets[i + 1];
        return std::string_view(name_block.data + off1, off2 - off1);
    }
};

template <class Enum>
constexpr lowercase_enum_name_list<Enum> lowercase_enum_names {};
} // namespace nav

#endif
