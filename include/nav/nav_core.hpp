#ifndef NAV_NAV_CORE
#define NAV_NAV_CORE

#include <array>
#include <optional>
#include <string_view>

#ifndef NAV_ADD_NULL_TERMINATORS
#define NAV_ADD_NULL_TERMINATORS 0
#endif

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

// Functions to get the name of a type at compile time
namespace nav {
template <class T>
constexpr std::string_view get_name_of_type() {
#ifdef __GNUC__
    // GCC: constexpr name_of_type get_name_of_type() [with T = int]
    // Clang
    constexpr std::string_view name = __PRETTY_FUNCTION__;
    size_t start = name.find("T = ") + 4;
#if defined(__clang__) || defined(__INTEL_COMPILER)
    size_t end = name.rfind("]");
#else
    size_t end = name.rfind("; std::string_view = ");
#endif
    return name.substr(start, end - start);
#elif _MSC_VER
    constexpr std::string_view name = __FUNCSIG__;
    std::string_view start_token = "get_name_of_type<";
    size_t start = name.find(start_token) + start_token.size();
    size_t end = name.rfind(">(void)");
    return name.substr(start, end - start);
#else
    return "<unknown type>";
#endif
};
template <class T>
constexpr std::string_view name_of_type = get_name_of_type<T>();
} // namespace nav

namespace nav::detail {
template <class BaseT>
struct enum_maker {
    BaseT value {};
    bool is_set = false;
    enum_maker() = default;
    enum_maker(enum_maker const&) = default;

    // Ignore values on creation. We only care about these on assignment
    constexpr enum_maker(BaseT)
      : enum_maker() {}

    // Copy assignment should operate like normal
    enum_maker& operator=(enum_maker const&) = default;

    // Assigning a value results in is_set being true
    template <class T>
    constexpr enum_maker& operator=(T const& v) {
        value = BaseT(v);
        is_set = true;
        return *this;
    }

    // clang-format off
    constexpr auto operator+() const { return +value; }
    constexpr auto operator-() const { return -value; }
    constexpr auto operator!() const { return !value; }
    constexpr auto operator~() const { return ~value; }
    template <class T>
    constexpr auto operator+(T other) const { return value + other; }
    template <class T>
    constexpr auto operator-(T other) const { return value - other; }
    template <class T>
    constexpr auto operator*(T other) const { return value * other; }
    template <class T>
    constexpr auto operator<<(T other) const { return value << other; }
    template <class T>
    constexpr auto operator>>(T other) const { return value >> other; }
    template <class T>
    constexpr auto operator<(T other) const { return value < other; }
    template <class T>
    constexpr auto operator>(T other) const { return value > other; }
    template <class T>
    constexpr auto operator<=(T other) const { return value <= other; }
    template <class T>
    constexpr auto operator>=(T other) const { return value >= other; }
    template <class T>
    constexpr auto operator=(T other) const { return value = other; }
    template <class T>
    constexpr auto operator==(T other) const { return value == other; }
    template <class T>
    constexpr auto operator!=(T other) const { return value != other; }
    template <class T>
    constexpr auto operator&(T other) const { return value & other; }
    template <class T>
    constexpr auto operator|(T other) const { return value | other; }
    template <class T>
    constexpr auto operator^(T other) const { return value ^ other; }
    template <class T>
    constexpr auto operator&&(T other) const { return value && other; }
    template <class T>
    constexpr auto operator||(T other) const { return value || other; }
    #if __cpp_impl_three_way_comparison >= 201907L
    template <class T>
    constexpr auto operator<=>(T other) const { return value <=> other; }
    #endif
    // clang-format on

    // Division and % have to be handled special, so that they're a no-op if
    // is_set isn't true
    template <class T>
    constexpr auto operator/(T other) const {
        return is_set ? value / other : value;
    }
    template <class T>
    constexpr auto operator%(T other) const {
        return is_set ? value % other : value;
    }

    constexpr operator BaseT() const {
        return value;
    }
    template <class T>
    constexpr explicit operator T() const {
        return T(value);
    }
};

template <class BaseT>
struct value_assigner {
    BaseT value {};

    constexpr value_assigner& operator,(enum_maker<BaseT>& other) {
        if (other.is_set) {
            // Update the current value to be the one stored in the enum
            value = other.value;
        } else {
            // set the enum value to be the current value
            other = value;
        }
        // Also we increment the value we hold
        value++;
        return *this;
    }
};

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
#if NAV_ADD_NULL_TERMINATORS
    return total + N;
#else
    return total;
#endif
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
        *offsets++ = current_offset;
#if NAV_ADD_NULL_TERMINATORS
        dest[size] = '\0';
        dest += size + 1;
        current_offset += size + 1;
#else
        dest += size;
        current_offset += size;
#endif
    });
    // The last offset holds the total length of the name block.
    *offsets = current_offset;
}

constexpr auto get_top_name(std::string_view view) {
    auto pos = view.find_last_of(':');
    if (pos == view.npos) {
        return view;
    } else {
        return view.substr(pos + 1);
    }
}
template <size_t N>
constexpr auto get_top_name(char const (&str)[N]) {
    return get_top_name(std::string_view(str, N - 1));
}
} // namespace nav::detail

namespace nav {
class string_block_iterator {
    char const* data {};
    unsigned const* indices {};

   public:
    string_block_iterator() = default;
    string_block_iterator(string_block_iterator const&) = default;
    constexpr string_block_iterator(
        char const* data,
        unsigned const* indices) noexcept
      : data(data)
      , indices(indices) {}

    constexpr string_block_iterator& operator++() noexcept {
        indices++;
        return *this;
    }
    constexpr string_block_iterator operator++(int) noexcept {
        string_block_iterator previous_state = *this;
        indices++;
        return previous_state;
    }
    constexpr intptr_t operator-(
        string_block_iterator const& other) const noexcept {
        return indices - other.indices;
    }
    constexpr bool operator==(
        string_block_iterator const& other) const noexcept {
        return indices == other.indices;
    }
    constexpr bool operator!=(
        string_block_iterator const& other) const noexcept {
        return indices != other.indices;
    }
    constexpr auto operator*() const noexcept -> std::string_view {
        int off0 = indices[0];
        int off1 = indices[1];
#if NAV_ADD_NULL_TERMINATORS
        return std::string_view(data + off0, off1 - off0 - 1);
#else
        return std::string_view(data + off0, off1 - off0);
#endif
    }
};

template <size_t N, size_t BlockSize>
struct string_block {
    static_assert(
        BlockSize >= N,
        "The size of the block must be greater than or equal to the number of "
        "strings");
    char data[BlockSize] {};
    unsigned offsets[N + 1] {};
    string_block() = default;
    string_block(string_block const&) = default;

    /**
     * @brief Constructs a string block by applying a given function to the
     * block (allows blocks to be constructed in-place)
     *
     * @tparam Fn a function with the signature (string_block&) -> void
     */
    template <class Fn>
    constexpr string_block(Fn&& func)
      : string_block() {
        func(*this);
    }

    /**
     * @brief Constructs a string block by mapping a source block to this block
     * using the given function (allows block to be constructed in-place)
     *
     * @tparam Fn a function with the signature (string_block const&,
     * string_block&) -> void
     */
    template <class Fn>
    constexpr string_block(string_block const& source, Fn&& func)
      : string_block() {
        func(source, *this);
    }

    using iterator = string_block_iterator;
    using const_iterator = iterator;
    constexpr iterator begin() const noexcept {
        return iterator(data, offsets);
    }
    constexpr iterator end() const noexcept {
        return iterator(data, offsets + N);
    }
    constexpr static size_t size() noexcept {
        return N;
    }
    constexpr static size_t block_size() noexcept {
        return BlockSize;
    }
    constexpr std::string_view operator[](size_t i) const noexcept {
        auto off1 = offsets[i];
#if NAV_ADD_NULL_TERMINATORS
        auto off2 = offsets[i + 1] - 1;
#else
        auto off2 = offsets[i + 1];
#endif
        return std::string_view(data + off1, off2 - off1);
    }
};
} // namespace nav

namespace nav::detail {
// Base type for enum_type_info
template <class Enum>
struct enum_type_info_base {
    constexpr static std::string_view qualified_type_name {name_of_type<Enum>};
    constexpr static std::string_view type_name {
        get_top_name(name_of_type<Enum>)};
    constexpr static size_t num_states = 0;
    constexpr static bool is_nav_enum = false;
    constexpr static size_t size() noexcept {
        return 0;
    }
};
// Base type for enum_values
template <class Enum>
struct enum_value_list_base : enum_type_info_base<Enum> {
    Enum __nav_internal_values__[0] {};
};
// Base type for enum_names
template <class Enum>
struct enum_name_list_base : enum_type_info_base<Enum> {
    using enum_type_info_base<Enum>::num_states;
    constexpr static size_t name_block_size = 0;
    using block_type = string_block<num_states, name_block_size>;
    block_type name_block;
};
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
        return values.__nav_internal_values__;
    }
    constexpr Enum const* end() const noexcept {
        return values.__nav_internal_values__ + super::num_states;
    }
    constexpr Enum const& operator[](size_t i) const noexcept {
        return values.__nav_internal_values__[i];
    }
};
template <class Enum>
struct enum_name_list : enum_type_info<Enum> {
   private:
    constexpr static detail::enum_name_list_base<Enum> name_info {};
    using super = enum_type_info<Enum>;

   public:
    using block_type = typename detail::enum_name_list_base<Enum>::block_type;
    using iterator = string_block_iterator;
    using const_iterator = iterator;
    constexpr static block_type const& get_name_block() noexcept {
        return name_info.name_block;
    }
    constexpr iterator begin() const noexcept {
        return name_info.name_block.begin();
    }
    constexpr iterator end() const noexcept {
        return name_info.name_block.end();
    }
    constexpr std::string_view operator[](size_t i) const noexcept {
        return name_info.name_block[i];
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

          }) {}                                                                        \
#define __NAV_CONCAT___(str1, str2) (str1 "::" str2)
#define __NAV_CONCAT__(str1, str2) (#str1 "::" #str2)

#define NAV_DECLARE_COMMON_ENUM(NamespaceName, EEnumType, BaseType, ...)                                     \
    namespace nav::detail {                                                                                  \
    template <>                                                                                              \
    struct enum_type_info_base<EnumType> {                                                                   \
        using base_type = BaseType;                                                                          \
        constexpr static std::string_view qualified_type_name = __NAV_CONCAT__(NamespaceName, EnumType);     \
        constexpr static std::string_view type_name = get_top_name(__NAV_CONCAT__(NamespaceName, EnumType)); \
        constexpr static bool is_nav_enum = true;                                                            \
        constexpr static size_t num_states = []() -> size_t {                                                \
            enum_maker<BaseType> __VA_ARGS__;                                                                \
            enum_maker<BaseType> NAV_DECLARE_ENUM_vals[] {__VA_ARGS__};                                      \
            return sizeof(NAV_DECLARE_ENUM_vals)                                                             \
                 / sizeof(enum_maker<BaseType>);                                                             \
        }();                                                                                                 \
        constexpr static size_t size() noexcept {                                                            \
            return num_states;                                                                               \
        }                                                                                                    \
    };                                                                                                       \
    template <>                                                                                              \
    struct enum_value_list_base<EnumType> : enum_type_info_base<EnumType> {                                  \
        EnumType __nav_internal_values__[enum_type_info_base<EnumType>::num_states];                         \
        constexpr enum_value_list_base()                                                                     \
          : __nav_internal_values__() {                                                                      \
            enum_maker<BaseType> __VA_ARGS__;                                                                \
            value_assigner<BaseType> {}, __VA_ARGS__;                                                        \
            enum_maker<BaseType> NAV_DECLARE_ENUM_vals[] {__VA_ARGS__};                                      \
            for (size_t i = 0; i < enum_type_info_base<EnumType>::num_states;                                \
                 i++)                                                                                        \
                this->__nav_internal_values__[i] = EnumType(NAV_DECLARE_ENUM_vals[i]);                       \
        }                                                                                                    \
    };                                                                                                       \
    template <>                                                                                              \
    struct enum_name_list_base<EnumType> : enum_type_info_base<EnumType> {                                   \
        using enum_type_info_base<EnumType>::num_states;                                                     \
        constexpr static size_t                                                                              \
            name_block_size = compute_name_block_size<num_states>(                                           \
                #__VA_ARGS__);                                                                               \
        using block_type = string_block<num_states, name_block_size>;                                        \
        block_type name_block;                                                                               \
        constexpr enum_name_list_base()                                                                      \
          : name_block([](auto& block) {                                                                     \
              write_names_and_sizes<num_states>(                                                             \
                  #__VA_ARGS__,                                                                              \
                  block.data,                                                                                \
                  block.offsets);                                                                            \
          }) {}                                                                                              \
    };                                                                                                       \
    } // namespace nav::detail

#define NAV_DECLARE_ENUM(NamespaceName, EnumType, BaseType, ...)                                             \
    namespace NamespaceName { enum EnumType : BaseType { __VA_ARGS__ }; }                                    \
    NAV_DECLARE_COMMON_ENUM(NamespaceName, EnumType, BaseType, __VA_ARGS__)

#define NAV_DECLARE_ENUM_CLASS(NamespaceName, EnumType, BaseType, ...)                                       \
    namespace NamespaceName { enum class EnumType : BaseType { __VA_ARGS__ }; }                              \
    NAV_DECLARE_COMMON_ENUM(NamespaceName, EnumType, BaseType, __VA_ARGS__)

#define NAV_DECLARE_ENUM_STRUCT(NamespaceName, EnumType, BaseType, ...)                                      \
    namespace NamespaceName { enum struct EnumType : BaseType { __VA_ARGS__ }; }                             \
    NAV_DECLARE_COMMON_ENUM(NamespaceName, EnumType, BaseType, __VA_ARGS__)

#ifndef NAV_NO_PRETTY_MACROS
#define nav_declare_enum(NamespaceName, EnumType, BaseType, ...)                                             \
    NAV_DECLARE_ENUM(NamespaceName, EnumType, BaseType, __VA_ARGS__)

#define nav_declare_enum_class(NamespaceName, EnumType, BaseType, ...)                                       \
    NAV_DECLARE_ENUM_CLASS(NamespaceName, EnumType, BaseType, __VA_ARGS__)

#define nav_declare_enum_struct(NamespaceName, EnumType, BaseType, ...)                                      \
  NAV_DECLARE_ENUM_STRUCT(NamespaceName, EnumType, BaseType, __VA_ARGS__)
#endif
#endif
