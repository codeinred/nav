#pragma once
#include "core.hpp"
#include <array>
#include <optional>
#include <string_view>

// TODO: Add function declarations + rewrite in terms of function declarations
namespace nav {
/**
 * @brief Return the declared name of a value in an enumeration, and store it in
 * `dest`. Return true on success, and false on failure. On failure, dest is
 * unmodified.
 *
 * @tparam Enum the enumeration type
 * @param value the value to name
 * @param dest the place to store the name
 * @param use_lowercase whether or not to use lowercase for the name
 * @return true on success
 * @return false on failure
 */
template <class Enum>
[[nodiscard]] constexpr bool name_of(
    Enum value,
    std::string_view& dest,
    bool use_lowercase = false) noexcept;

/**
 * @brief Return the declared name of a value in an enumeration. Return an
 * optional containing the value on success, and return a null optional on
 * failure.
 *
 * @tparam Enum the enueration type
 * @param value the value to name
 * @param use_lowercase whether or not to use lowercase for the name
 * @return std::optional<std::string_view>
 */
template <class Enum>
constexpr auto name_of(Enum value, bool use_lowercase = false) noexcept
    -> std::optional<std::string_view>;

/**
 * @brief Get the value corresponding to a name, and store it in `dest`. Return
 * true on success, and false on failure. On failure, dest is unmodified.
 *
 * @tparam Enum the enumeration type
 * @param name the name
 * @param dest the destination
 * @param ignore_case whether or not to ignore case when looking up the name
 * @return true on success
 * @return false on failure
 */
template <class Enum>
[[nodiscard]] constexpr bool value_of(
    std::string_view name,
    Enum& dest,
    bool ignore_case = false) noexcept;

/**
 * @brief Get the value corresponding to a name. Return an optional containing
 * the value on success, and return a null optional on failure.
 *
 * @tparam Enum the enumeration type
 * @param name the name
 * @param dest the destination
 * @param ignore_case whether or not to ignore case when looking up the name
 * @return some value, or nothing
 */
template <class Enum>
constexpr auto value_of(
    std::string_view name,
    bool ignore_case = false) noexcept -> std::optional<Enum>;
} // namespace nav

namespace nav::detail {
constexpr std::string_view trim_whitespace(std::string_view view) {
    size_t start = view.find_first_not_of(' ');
    if (start == std::string_view::npos) {
        return "";
    } else {
        size_t end = view.find_first_of(" =", start);
        if (end == std::string_view::npos) {
            return view.substr(start);
        } else {
            return view.substr(start, end - start);
        }
    }
}
template <size_t N, size_t StrSize, class Fn>
constexpr void split_trim_apply(char const (&str)[StrSize], Fn func) {
    std::string_view view(str, StrSize - 1);

    for (size_t i = 0; i < N; i++) {
        auto new_pos = view.find(',');
        func(trim_whitespace(view.substr(0, new_pos)));
        if (new_pos != std::string_view::npos) {
            view = view.substr(new_pos + 1);
        } else {
            break;
        }
    }
}
template <size_t N, size_t StrSize>
constexpr size_t compute_name_block_size(char const (&str)[StrSize]) {
    size_t total = 0;
    split_trim_apply<N>(str, [&](std::string_view sv) { total += sv.size(); });
    // The name block size is the total count + N, because each name is null
    // terminated.
    return total + N;
}
// Copy characters into the destination buffer and store the offsets in the
// offsets variable. Names are separated by a '\0' character for compatibility
// with C.
template <size_t N, size_t StrSize>
constexpr void write_names_and_sizes(
    char const (&str)[StrSize],
    char* dest,
    unsigned* offsets) {
    unsigned current_offset = 0;
    split_trim_apply<N>(str, [&](std::string_view sv) {
        char const* source = sv.data();
        size_t size = sv.size();
        for (size_t i = 0; i < size; i++) {
            dest[i] = source[i];
        }
        dest[size] = '\0';
        *offsets++ = current_offset;
        dest += size + 1;
        current_offset += size + 1;
    });
    // The last offset holds the total length of the name block.
    *offsets = current_offset;
}
} // namespace nav::detail

namespace nav::detail {
// Base type for enum_type_info
template <class Enum>
struct enum_type_info_base {
    constexpr static size_t size = 0;
    constexpr static bool is_nav_enum = false;
};
// Base type for enum_values
template <class Enum>
struct enum_value_list_base {};
// Base type for enum_names
template <class Enum>
struct enum_name_list_base {};
} // namespace nav::detail

namespace nav {
template <class Enum>
struct enum_type_info : public detail::enum_type_info_base<Enum> {
   private:
    using super = detail::enum_type_info_base<Enum>;

   public:
    using super::is_nav_enum;
    using super::num_states;
    using super::qualified_type_name;
    using super::size;
    using super::type_name;
};
template <class Enum>
struct enum_value_list : enum_type_info<Enum> {
   private:
    constexpr static detail::enum_value_list_base<Enum> values {};
    using super = enum_type_info<Enum>;

   public:
    using iterator = Enum const*;
    using const_iterator = Enum const*;
    constexpr Enum const* begin() const noexcept {
        return values.values;
    }
    constexpr Enum const* end() const noexcept {
        return values.values + super::num_states;
    }
    constexpr Enum const& operator[](size_t i) const noexcept {
        return values.values[i];
    }
};
template <class Enum>
struct enum_name_list : enum_type_info<Enum> {
   private:
    constexpr static detail::enum_name_list_base<Enum> name_info {};
    using super = enum_type_info<Enum>;

   public:
    class iterator {
        unsigned off1 {};
        unsigned off2 {};
        unsigned const* index {};

       public:
        iterator() = default;
        iterator(iterator const&) = default;
        constexpr iterator(unsigned const* index) noexcept
          : off1(index[0])
          , off2(index[1])
          , index(index) {}

        constexpr iterator& operator++() noexcept {
            index++;
            off1 = off2;
            off2 = index[1];
            return *this;
        }
        constexpr iterator operator++(int) noexcept {
            iterator previous_state = *this;
            index++;
            off1 = off2;
            off2 = *index;
            return previous_state;
        }
        constexpr intptr_t operator-(iterator const& other) const noexcept {
            return index - other.index;
        }
        constexpr bool operator==(iterator const& other) const noexcept {
            return index == other.index;
        }
        constexpr bool operator!=(iterator const& other) const noexcept {
            return index != other.index;
        }
        constexpr auto operator*() const noexcept -> std::string_view {
            return std::string_view(
                name_info.name_block + off1,
                off2 - off1 - 1);
        }
    };
    using const_iterator = iterator;
    constexpr iterator begin() const noexcept {
        return iterator(name_info.name_offsets);
    }
    constexpr iterator end() const noexcept {
        return iterator(name_info.name_offsets + super::num_states);
    }
    constexpr std::string_view operator[](size_t i) const noexcept {
        auto off1 = name_info.name_offsets[i];
        auto off2 = name_info.name_offsets[i + 1] - 1;
        return std::string_view(name_info.name_block + off1, off2 - off1);
    }
};

template <class Enum>
constexpr bool is_nav_enum = enum_type_info<Enum>::is_nav_enum;

template <class Enum>
constexpr size_t num_states = enum_type_info<Enum>::num_states;

template <class Enum>
constexpr enum_value_list<Enum> enum_values {};

template <class Enum>
constexpr enum_name_list<Enum> enum_names {};
} // namespace nav

#define nav_declare_enum(EnumType, BaseType, ...)                              \
    enum class EnumType : BaseType { __VA_ARGS__ };                            \
    namespace nav::detail {                                                    \
    template <>                                                                \
    struct enum_type_info_base<EnumType> {                                     \
        using base_type = BaseType;                                            \
        constexpr static std::string_view qualified_type_name = #EnumType;     \
        constexpr static std::string_view type_name = get_top_name(#EnumType); \
        constexpr static bool is_nav_enum = true;                              \
        constexpr static size_t num_states = []() -> size_t {                  \
            enum_maker<BaseType> __VA_ARGS__;                                  \
            enum_maker<BaseType> NAV_DECLARE_ENUM_vals[] {__VA_ARGS__};        \
            return sizeof(NAV_DECLARE_ENUM_vals)                               \
                 / sizeof(enum_maker<BaseType>);                               \
        }();                                                                   \
        constexpr static size_t size() noexcept {                              \
            return num_states;                                                 \
        }                                                                      \
    };                                                                         \
    template <>                                                                \
    struct enum_value_list_base<EnumType> : enum_type_info_base<EnumType> {    \
        EnumType values[enum_type_info_base<EnumType>::num_states];            \
        constexpr enum_value_list_base()                                       \
          : values() {                                                         \
            enum_maker<BaseType> __VA_ARGS__;                                  \
            value_assigner<BaseType> {}, __VA_ARGS__;                          \
            enum_maker<BaseType> NAV_DECLARE_ENUM_vals[] {__VA_ARGS__};        \
            for (size_t i = 0; i < enum_type_info_base<EnumType>::num_states;  \
                 i++)                                                          \
                this->values[i] = EnumType(NAV_DECLARE_ENUM_vals[i]);          \
        }                                                                      \
    };                                                                         \
    template <>                                                                \
    struct enum_name_list_base<EnumType> : enum_type_info_base<EnumType> {     \
        using enum_type_info_base<EnumType>::num_states;                       \
        constexpr static size_t                                                \
            name_block_size = compute_name_block_size<num_states>(             \
                #__VA_ARGS__);                                                 \
        char name_block[name_block_size];                                      \
        unsigned name_offsets[num_states + 1];                                 \
        constexpr enum_name_list_base()                                        \
          : name_block()                                                       \
          , name_offsets() {                                                   \
            write_names_and_sizes<num_states>(                                 \
                #__VA_ARGS__,                                                  \
                name_block,                                                    \
                name_offsets);                                                 \
        }                                                                      \
    };                                                                         \
    } // namespace nav::detail
