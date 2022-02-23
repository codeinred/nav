#pragma once
#include <array>
#include <optional>
#include <string_view>

namespace nav {
// Small convinient view class
template <class T>
struct view {
    constexpr static auto npos = ~size_t(0);
    T const* _begin {};
    T const* _end {};
    view() = default;
    view(view const&) = default;
    template <size_t N>
    constexpr view(T const (&arr)[N]) noexcept
      : _begin(arr)
      , _end(arr + N) {}
    template <size_t N>
    constexpr view(std::array<T, N> const& arr) noexcept
      : _begin(arr.data())
      , _end(arr.data() + N) {}
    constexpr view(T const* begin, T const* end) noexcept
      : _begin(begin)
      , _end(end) {}
    constexpr view(T const* begin, size_t N) noexcept
      : _begin(begin)
      , _end(begin + N) {}
    constexpr T const* begin() const {
        return _begin;
    }
    constexpr T const* end() const {
        return _end;
    }
    constexpr size_t size() const noexcept {
        return _end - _begin;
    }
    constexpr intptr_t ssize() const noexcept {
        return _end - _begin;
    }
    constexpr T const& operator[](size_t i) const noexcept {
        return _begin[i];
    }
};

// Represents a key-value pair in a map
template <class Key, class Value>
struct map_entry {
    Key key {};
    Value value {};
};
} // namespace nav

// Enum maker - used to compute enum values. This allows us to avoid
// shennanigans involving recursive  macro expansions.
namespace nav::impl {
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

template <class T>
constexpr T max(T const& a, T const& b) {
    return a < b ? b : a;
}
template <size_t N, class T>
constexpr T max_elem(T const* ptr) {
    if constexpr (N == 0) {
        return T {};
    } else {
        T max = ptr[0];
        for (size_t i = 1; i < N; i++) {
            if (ptr[i] > max) {
                max = ptr[i];
            }
        }
        return max;
    }
}
template <class T>
constexpr auto compare(T const& a, T const& b) {
#if __cpp_impl_three_way_comparison >= 201907L
    return a <=> b;
#else
    if (a < b) {
        return -1;
    } else if (a > b) {
        return 1;
    } else {
        return 0;
    }
#endif
}
constexpr auto compare(std::string_view a, std::string_view b) {
    return a.compare(b);
}
template <class K, class V>
constexpr auto compare(map_entry<K, V> const& entry, K const& other) {
    return compare(entry.key, other);
}
template <class K, class V>
constexpr auto compare(map_entry<K, V> const& a, map_entry<K, V> const& b) {
    return compare(a.key, b.key);
}
template <class T, class K>
constexpr auto binary_search(view<T> vals, K key) -> T const* {
    size_t lower_i = 0;
    size_t upper_i = vals.size() - 1;
    while (lower_i <= upper_i) {
        size_t i = (lower_i + upper_i) / 2;
        auto cmp = compare(vals[i], key);

        if (cmp < 0) {
            lower_i = i + 1;
        } else if (cmp > 0) {
            upper_i = i - 1;
        } else {
            return &vals[i];
        }
    }
    return nullptr;
}
template <class T>
constexpr void swap(T& a, T& b) {
    auto Tmp = static_cast<T&&>(a);
    a = static_cast<T&&>(b);
    b = static_cast<T&&>(Tmp);
}
constexpr auto to_lower = [](char ch) -> char {
    return 'A' <= ch && ch <= 'Z' ? ch - 'A' + 'a' : ch;
};
template <class Key, class Value, class BaseT, BaseT Min, BaseT Max>
class indexed_map {
    constexpr static size_t ArraySize = Max - Min + 1;
    std::array<Value, ArraySize> vals {};
    std::array<bool, ArraySize> present {};

    Value default_value;

   public:
    template <size_t N>
    constexpr indexed_map(
        std::array<Key, N> const& keys,
        std::array<Value, N> const& values,
        Value default_value)
      : vals()
      , present()
      , default_value(default_value) {
        for (auto& val : vals) {
            val = default_value;
        }
        for (size_t i = 0; i < N; i++) {
            auto key_i = BaseT(keys[i]) - Min;
            if (!present[key_i]) {
                vals[key_i] = values[i];
                present[key_i] = true;
            }
        }
    }
    constexpr auto contains(Key key) const -> bool {
        auto i = BaseT(key);
        if (i < Min || i > Max) {
            return false;
        } else {
            return present[i - Min];
        }
    }
    constexpr auto get(Key key) const -> std::optional<Value> {
        return (*this)[key];
    }
    constexpr auto get(Key key, Value default_value) const -> Value {
        if (std::optional<Value> result = (*this)[key]) {
            return *result;
        } else {
            return default_value;
        }
    }
    constexpr auto operator[](Key key) const -> std::optional<Value> {
        auto i = BaseT(key);
        if (i < Min || i > Max || !present[i - Min]) {
            return std::nullopt;
        } else {
            return vals[i - Min];
        }
    }
};

// Stably sorts and de-duplicates an array, returning a new number of elements
// stably sorted with duplicates removed
template <size_t N, class T>
constexpr void sort(T* values) {
    using impl::compare;
    if constexpr (N <= 1) {
        return;
    } else if constexpr (N == 2) {
        auto& a = values[0];
        auto& b = values[1];
        if (compare(b, a) < 0) {
            impl::swap(a, b);
        }
        return;
    } else {
        constexpr size_t first_half = N / 2;
        constexpr size_t second_half = N - N / 2;
        T* a1 = values;
        T* a2 = values + first_half;
        T* const end1 = values + first_half;
        T* const end2 = values + N;
        sort<first_half>(a1);
        sort<second_half>(a2);

        T buffer[N] {};

        T* dest = buffer;
        while (a1 < end1 && a2 < end2) {
            *dest++ = compare(*a2, *a1) < 0 ? *a2++ : *a1++;
        }
        if (a1 == end1) {
            while (a2 < end2) {
                *dest++ = *a2++;
            }
        } else {
            while (a1 < end1) {
                *dest++ = *a1++;
            }
        }
        // Copy the elements back from buffer to the original array
        for (size_t i = 0; i < N; i++) {
            values[i] = buffer[i];
        }
    }
}
// Stably sorts and de-duplicates an array, returning a new number of elements
// stably sorted with duplicates removed
template <size_t N, class T>
constexpr size_t sort_dedup(T* values) {
    using impl::compare;
    if constexpr (N <= 1) {
        return N;
    } else if constexpr (N == 2) {
        auto& a = values[0];
        auto& b = values[1];
        if (compare(a, b) < 0) {
            return N;
        } else if (compare(b, a) < 0) {
            impl::swap(a, b);
            return N;
        } else {
            // We return 1 b/c we're discarding B
            return 1;
        }
    } else {
        size_t first_half = sort_dedup<N / 2>(values);
        size_t second_half = sort_dedup<N - N / 2>(values + N / 2);

        T buffer[N];
        size_t count = 0;

        {
            T* dest = buffer;
            T* a1 = values;
            T* a2 = values + N / 2;
            T* const end1 = a1 + first_half;
            T* const end2 = a2 + second_half;
            while (a1 < end1 && a2 < end2) {
                int cmp = compare(*a1, *a2);
                if (cmp < 0) {
                    *dest++ = *a1++;
                } else if (cmp > 0) {
                    *dest++ = *a2++;
                } else {
                    // Discard the element from a2, since that's the duplicate
                    *dest++ = *a1++;
                    a2++;
                }
            }
            if (a1 == end1) {
                while (a2 < end2) {
                    *dest++ = *a2++;
                }
            } else {
                while (a1 < end1) {
                    *dest++ = *a1++;
                }
            }
            count = dest - buffer;
        }
        // Copy the elements back from buffer to the original array
        for (size_t i = 0; i < count; i++) {
            values[i] = buffer[i];
        }
        // Return the number of elements we placed in the resulting array
        return count;
    }
}
template <class Key, class Value, size_t N>
class binary_dedup_map {
    using Entry = map_entry<Key, Value>;
    Value default_value {};
    std::array<Entry, N> entries;
    size_t count = 0;
    // count <= N (It's the count with duplicates removed)


   public:
    constexpr binary_dedup_map(
        std::array<Key, N> const& keys,
        std::array<Value, N> const& values,
        Value const& default_value)
      : default_value(default_value) {
        for (size_t i = 0; i < N; i++) {
            entries[i] = {keys[i], values[i]};
        }
        // Stably sort and deduplicate entries. Compute count to be the number
        // of entries after deduplication
        count = sort_dedup<N>(entries.data());
    }
    constexpr bool contains(Key key) const {
        return binary_search(view(entries.data(), count), key) != nullptr;
    }
    constexpr auto get(Key key) const -> std::optional<Value> {
        return (*this)[key];
    }
    constexpr auto get(Key key, Value default_value) const -> Value {
        if (std::optional<Value> result = (*this)[key]) {
            return *result;
        } else {
            return default_value;
        }
    }
    constexpr auto operator[](Key key) const -> std::optional<Value> {
        if(Entry const* entry = binary_search(view(entries.data(), count), key)) {
            return entry->value;
        } else {
            return std::nullopt;
        }
    }
};

// A map that returns an optional when you index into it. Returns nullopt if the
// item wasn't found.
template <class Key, class Value, size_t N>
class binary_map {
    using Entry = map_entry<Key, Value>;
    std::array<Entry, N> entries;
    // count <= N (It's the count with duplicates removed)


   public:
    constexpr binary_map(
        std::array<Key, N> const& keys,
        std::array<Value, N> const& values) {
        for (size_t i = 0; i < N; i++) {
            entries[i] = {keys[i], values[i]};
        }
        // Stably sort and deduplicate entries. Compute count to be the number
        // of entries after deduplication
        impl::sort<N>(entries.data());
    }
    constexpr bool contains(Key key) const {
        return binary_search(view(entries), key) != nullptr;
    }
    constexpr auto get(Key key) const -> std::optional<Value> {
        return (*this)[key];
    }
    constexpr auto get(Key key, Value default_value) const -> Value {
        if (std::optional<Value> result = (*this)[key]) {
            return *result;
        } else {
            return default_value;
        }
    }
    constexpr auto operator[](Key key) const -> std::optional<Value> {
        if(Entry const* entry = binary_search(view(entries), key)) {
            return entry->value;
        } else {
            return std::nullopt;
        }
    }

    constexpr view<Entry> get_entries() const {
        return {entries.data(), entries.data() + N};
    }
    constexpr bool validate_map() const {
        for (size_t i = 1; i < entries.size(); i++) {
            if (entries[i - 1].key >= entries[i].key) {
                return false;
            }
        }
        return true;
    }
};

template <class Key, class Value, size_t N, class BaseT, BaseT Min, BaseT Max>
constexpr auto select_map(
    std::array<Key, N> const& keys,
    std::array<Value, N> const& values,
    Value const& default_value) {
    // If we have a large sparse map, select binary_dedup_map. Otherwise, select
    // the indexed map, which should be faster, but may use more memory
    if constexpr (Max - Min > 2 * N + 256) {
        return binary_dedup_map<Key, Value, N>(keys, values, default_value);
    } else {
        return indexed_map<Key, Value, BaseT, Min, Max>(
            keys,
            values,
            default_value);
    }
}

template <class BaseT, class T, size_t N>
constexpr auto min_base_value(std::array<T, N> const& arr) -> BaseT {
    if constexpr (N == 0) {
        return BaseT();
    } else {
        auto min_value = BaseT(arr[0]);
        for (size_t i = 1; i < N; i++) {
            min_value = std::min(min_value, BaseT(arr[i]));
        }
        return min_value;
    }
}
template <class BaseT, class T, size_t N>
constexpr auto max_base_value(std::array<T, N> const& arr) -> BaseT {
    if constexpr (N == 0) {
        return BaseT();
    } else {
        auto max_value = BaseT(arr[0]);
        for (size_t i = 1; i < N; i++) {
            max_value = max(max_value, BaseT(arr[i]));
        }
        return max_value;
    }
}
template <class T, class... Args>
constexpr auto make_array(Args&&... args) -> std::array<T, sizeof...(args)> {
    return {static_cast<Args&&>(args)...};
}
template <size_t BuffSize, size_t N>
constexpr auto static_cat(
    std::array<std::string_view, N> const& views,
    char sep) -> std::array<char, BuffSize> {
    std::array<char, BuffSize> arr {};
    size_t i = 0;
    for (auto view : views) {
        for (char ch : view) {
            arr[i++] = ch;
        }
        arr[i++] = sep;
    }
    return arr;
}
template <size_t N>
constexpr auto split_by_lengths_assuming_sep(
    std::array<size_t, N> const& lengths,
    char const* data) -> std::array<std::string_view, N> {
    std::array<std::string_view, N> segments;
    size_t offset = 0;
    for (size_t i = 0; i < N; i++) {
        segments[i] = std::string_view(data + offset, lengths[i]);
        offset += lengths[i] + 1;
    }
    return segments;
}

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
template <size_t N, size_t StrSize>
constexpr auto split_trim(char const (&str)[StrSize])
    -> std::array<std::string_view, N> {
    std::string_view view(str, StrSize - 1);
    std::array<std::string_view, N> args;

    size_t start_pos = 0;
    for (size_t i = 0; i < N; i++) {
        auto new_pos = view.find(',');
        args[i] = trim_whitespace(view.substr(0, new_pos));
        if (new_pos != std::string_view::npos) {
            view = view.substr(new_pos + 1);
        } else {
            break;
        }
    }
    return args;
}
template <size_t N>
constexpr auto get_top_name(char const (&str)[N]) {
    std::string_view view(str, N - 1);
    auto pos = view.find_last_of(':');
    if (pos == view.npos) {
        return view;
    } else {
        return view.substr(pos + 1);
    }
}

template <
    class T,
    size_t N,
    class F,
    class Ret = decltype(std::declval<F>()(std::declval<T>()))>
constexpr auto map_array(std::array<T, N> const& arr, F func)
    -> std::array<Ret, N> {
    std::array<Ret, N> result {};
    for (size_t i = 0; i < N; i++) {
        result[i] = func(arr[i]);
    }
    return result;
}
template <class Elem, class Result, class F>
constexpr Result fold(Elem const* values, size_t N, Result initial, F func) {
    for (size_t i = 0; i < N; i++) {
        initial = func(initial, values[i]);
    }
    return initial;
}
template <class Enum>
struct traits_impl {
    // Allows checks for the existence of nav enum traits
    constexpr static bool is_nav_enum = false;
};


/**
 * @brief Compute the smallest number of the form 2^N-1 such that 2^N-1 >= i
 *
 * @param i
 * @return constexpr size_t
 */
constexpr size_t bit_ceil_minus_1(size_t i) {
    size_t ceil = 0;
    while (ceil < i) {
        ceil <<= 1;
        ceil |= 1;
    }
    return ceil;
}
} // namespace nav::impl

namespace nav {
template <class EnumType>
struct enum_traits : private impl::traits_impl<EnumType> {
   private:
    using super = impl::traits_impl<EnumType>;

   public:
    using enum_type = EnumType;
    using base_type = typename super::base_type;
    using super::is_nav_enum;
    using super::qualified_type_name;
    using super::type_name;
    using super::values;
    constexpr static auto size = values.size();
    constexpr static auto name_lengths = impl::map_array(
        super::names_raw,
        [](std::string_view name) { return name.size(); });

   private:
    constexpr static size_t name_block_buffer_size = size
                                                   + impl::fold(
                                                         name_lengths.data(),
                                                         size,
                                                         0,
                                                         [](size_t acc,
                                                            size_t elem) {
                                                             return acc + elem;
                                                         });
    constexpr static auto name_block = impl::static_cat<name_block_buffer_size>(
        super::names_raw,
        '\0');
    constexpr static auto lowercase_name_block = map_array(
        name_block,
        impl::to_lower);

   public:
    /* A list of all the names in the enum, in declaration order */
    constexpr static auto names = impl::split_by_lengths_assuming_sep(
        name_lengths,
        name_block.data());

   public:
    /* All the enum names, but lowercase. Provided to support lookup
     * operations that ignore case. */
    constexpr static auto lowercase_names = impl::split_by_lengths_assuming_sep(
        name_lengths,
        lowercase_name_block.data());
    constexpr static size_t max_name_length = impl::max_elem<size>(
        name_lengths.data());
    constexpr static base_type min = impl::min_base_value<base_type>(values);
    constexpr static base_type max = impl::max_base_value<base_type>(values);
    constexpr static auto values_to_names = impl::
        select_map<EnumType, std::string_view, size, base_type, min, max>(
            values,
            names,
            "<unnamed>");
    constexpr static auto
        names_to_values = impl::binary_map<std::string_view, EnumType, size>(
            names,
            values);
    constexpr static auto lowercase_names_to_values = impl::
        binary_map<std::string_view, EnumType, size>(lowercase_names, values);
    constexpr static std::optional<EnumType> get_value(std::string_view name) {
        return names_to_values[name];
    }
    constexpr static EnumType get_value(
        std::string_view name,
        EnumType alternative) {
        return names_to_values.get(name, alternative);
    }
    constexpr static std::optional<EnumType> get_value_ignore_case(
        std::string_view name) {
        if (name.size() > max_name_length) {
            return std::nullopt;
        } else {
            char buffer[max_name_length + 1];
            for (size_t i = 0; i < name.size(); i++) {
                buffer[i] = impl::to_lower(name[i]);
            }
            buffer[name.size()] = '\0';
            return lowercase_names_to_values[std::string_view(
                buffer,
                names.size())];
        }
    }
    constexpr static EnumType get_value_ignore_case(
        std::string_view name,
        EnumType alternative) {
        if (name.size() > max_name_length) {
            return alternative;
        } else {
            char buffer[max_name_length + 1];
            for (size_t i = 0; i < name.size(); i++) {
                buffer[i] = impl::to_lower(name[i]);
            }
            buffer[name.size()] = '\0';
            return lowercase_names_to_values.get(
                std::string_view(buffer, names.size()),
                alternative);
        }
    }
    constexpr static std::optional<std::string_view> get_name(EnumType value) {
        return values_to_names[value];
    }
    constexpr static std::string_view get_name(
        EnumType value,
        std::string_view alternative) {
        return values_to_names.get(value, alternative);
    }
};

/**
 * @brief Get the number of declared states an enum has
 *
 * @tparam T the enum
 */
template <class T>
constexpr size_t enum_size = enum_traits<T>::size;

/**
 * @brief Get the values declared for an enum, in declaration order
 *
 * @tparam T the enum
 */
template <class T>
constexpr auto const& enum_values = enum_traits<T>::values;

/**
 * @brief Get the names of an enum's values, in declarattion order
 *
 * @tparam T the enum
 */
template <class T>
constexpr auto const& enum_names = enum_traits<T>::names;

// Checks if an enum has associated traits
template <class T>
constexpr bool is_nav_enum = impl::traits_impl<T>::is_nav_enum;

#if __cpp_concepts >= 201907L
template <class T>
concept nav_enum = is_nav_enum<T>;
#endif
} // namespace nav

#define nav_declare_enum(EnumType, BaseType, ...)                              \
    enum class EnumType : BaseType { __VA_ARGS__ };                            \
    namespace nav::impl {                                                      \
    template <>                                                                \
    struct traits_impl<EnumType> {                                             \
        constexpr static bool is_nav_enum = true;                              \
        friend class ::nav::enum_traits<EnumType>;                             \
        using base_type = BaseType;                                            \
        constexpr static std::string_view qualified_type_name = #EnumType;     \
        constexpr static std::string_view type_name = get_top_name(#EnumType); \
        /* A list of all the values in the enum, in declaration order */       \
        constexpr static auto values = []() {                                  \
            enum_maker<BaseType> __VA_ARGS__;                                  \
            value_assigner<BaseType> {}, __VA_ARGS__;                          \
            enum_maker<BaseType> NAV_DECLARE_ENUM_vals[] {__VA_ARGS__};        \
            {                                                                  \
                constexpr size_t N = sizeof(NAV_DECLARE_ENUM_vals)             \
                                   / sizeof(enum_maker<BaseType>);             \
                std::array<EnumType, N> result {};                             \
                for (size_t i = 0; i < N; i++)                                 \
                    result[i] = EnumType(NAV_DECLARE_ENUM_vals[i]);            \
                return result;                                                 \
            }                                                                  \
        }();                                                                   \
                                                                               \
       private:                                                                \
        constexpr static auto names_raw = split_trim<values.size()>(           \
            #__VA_ARGS__);                                                     \
    };                                                                         \
    } // namespace nav::impl