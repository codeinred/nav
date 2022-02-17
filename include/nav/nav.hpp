#pragma once
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include <type_traits>
#include <utility>

#define PARENS ()
#define EXPAND(...) EXPAND4(EXPAND4(EXPAND4(__VA_ARGS__)))
#define EXPAND4(...) EXPAND3(EXPAND3(EXPAND3(EXPAND3(__VA_ARGS__))))
#define EXPAND3(...) EXPAND2(EXPAND2(EXPAND2(EXPAND2(__VA_ARGS__))))
#define EXPAND2(...) EXPAND1(EXPAND1(EXPAND1(EXPAND1(__VA_ARGS__))))
#define EXPAND1(...) __VA_ARGS__

#define MAP_OPERATOR(func, op, ...)                                            \
    __VA_OPT__(EXPAND(MAP_OPERATOR_HELPER(func, op, __VA_ARGS__)))
#define MAP_OPERATOR_HELPER(func, op, a1, ...)                                 \
    func(op a1), __VA_OPT__(MAP_OPERATOR_AGAIN PARENS(func, op, __VA_ARGS__))
#define MAP_OPERATOR_AGAIN() MAP_OPERATOR_HELPER

// MAP_OPERATOR(!, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16)


namespace nav::impl {
constexpr auto to_lower = [](char ch) -> char {
    if ('A' <= ch && ch <= 'Z') {
        return ch - 'A' + 'a';
    } else {
        return ch;
    }
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
template <size_t N, class T, class Cmp>
constexpr size_t sort_dedup(T* values, Cmp less) {
    if constexpr (N <= 1)
        return N;

    if constexpr (N == 2) {
        auto &a = values[0], b = values[1];
        if (less(a, b)) {
            return N;
        } else if (less(b, a)) {
            std::swap(a, b);
            return N;
        } else {
            // We return 1 b/c we're discarding B
            return 1;
        }
    }

    size_t first_half = sort_dedup<N / 2>(values, less);
    size_t second_half = sort_dedup<N - N / 2>(values + N / 2, less);

    T buffer[N];

    T* a1 = values;
    T* a2 = values + N / 2;
    size_t i1 = 0, i2 = 0, dest = 0;
    for (;;) {
        // If either array is out of elements, copy the remaining elements and
        // then break
        if (i1 == first_half) {
            while (i2 < second_half) {
                buffer[dest++] = a2[i2++];
            }
            break;
        } else if (i2 == second_half) {
            while (i1 < first_half) {
                buffer[dest++] = a1[i1++];
            }
            break;
        } else {
            if (less(a1[i1], a2[i2])) {
                buffer[dest++] = a1[i1++];
            } else if (less(a2[i2], a1[i1])) {
                buffer[dest++] = a2[i2++];
            } else {
                // Discard the element from a2, since that's the duplicate
                buffer[dest++] = a1[i1++];
                i2++;
            }
        }
    }
    // Copy the elements back from buffer to the original array
    for (size_t i = 0; i < dest; i++) {
        values[i] = buffer[i];
    }
    // Return the number of elements we placed in the resulting array
    return dest;
}
template <class Key, class Value, size_t N>
class binary_dedup_map {
    struct Entry {
        Key key {};
        Value value {};
    };
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
        count = sort_dedup<N>(
            entries.data(),
            [](auto const& e1, auto const& e2) { return e1.key < e2.key; });
    }
    constexpr bool contains(Key key) const {
        size_t min = 0, max = count, i = count / 2;
        while (max - min > 1) {
            auto entry_key = entries[i];
            auto cmp = key <=> entries[i].key;
            if (cmp == 0) {
                return true;
            }
            if (cmp < 0) {
                max = i;
            } else {
                min = i;
            }
            i = (max + min) / 2;
        }
        return entries[i].key == key;
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
        size_t min = 0, max = count, i = count / 2;
        while (max - min > 1) {
            auto entry_key = entries[i];
            auto cmp = key <=> entries[i].key;
            if (cmp == 0) {
                return entries[i].value;
            }
            if (cmp < 0) {
                max = i;
            } else {
                min = i;
            }
            i = (max + min) / 2;
        }
        return entries[i].key == key ? std::optional<Value> {entries[i].value}
                                     : std::nullopt;
    }
};

// A map that returns an optional when you index into it. Returns nullopt if the
// item wasn't found.
template <class Key, class Value, size_t N>
class binary_map {
    struct Entry {
        Key key {};
        Value value {};
    };
    std::array<Entry, N> entries;
    size_t count = 0;
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
        std::sort(
            entries.data(),
            entries.data() + N,
            [](auto const& e1, auto const& e2) { return e1.key < e2.key; });
    }
    constexpr bool contains(Key key) const {
        size_t min = 0, max = count, i = count / 2;
        while (max - min > 1) {
            auto entry_key = entries[i];
            auto cmp = key <=> entries[i].key;
            if (cmp == 0) {
                return true;
            }
            if (cmp < 0) {
                max = i;
            } else {
                min = i;
            }
            i = (max + min) / 2;
        }
        return entries[i].key == key;
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
        size_t min = 0, max = count, i = count / 2;
        while (max - min > 1) {
            auto entry_key = entries[i];
            auto cmp = key <=> entries[i].key;
            if (cmp == 0) {
                return entries[i].value;
            }
            if (cmp < 0) {
                max = i;
            } else {
                min = i;
            }
            i = (max + min) / 2;
        }
        return entries[i].key == key ? std::optional<Value> {entries[i].value}
                                     : std::nullopt;
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
            max_value = std::max(max_value, BaseT(arr[i]));
        }
        return max_value;
    }
}
template <class EnumType>
struct EnumAssignmentGuard {
    EnumType value;

    constexpr auto operator=(auto&&) const {
        return EnumAssignmentGuard {value};
    }
    constexpr operator EnumType() const {
        return value;
    }
};
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
    std::array<Ret, N> result;
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
struct traits_impl {};
} // namespace nav::impl

namespace nav {
template <class Enum>
struct enum_traits : impl::traits_impl<Enum> {};
} // namespace nav

#define STRINGIFY_VARIADIC(count, args...) split_trim<count>(#args)
#define nav_declare_enum(EnumType, BaseType, ...)                              \
    enum class EnumType : BaseType { __VA_ARGS__ };                            \
    namespace nav::impl {                                                      \
    constexpr auto operator!(EnumType e) {                                     \
        return EnumAssignmentGuard<EnumType> {e};                              \
    }                                                                          \
    template <>                                                                \
    struct traits_impl<EnumType> {                                             \
        using enum EnumType;                                                   \
        using base_type = BaseType;                                            \
        using type = EnumType;                                                 \
        constexpr static std::string_view qualified_name = #EnumType;          \
        constexpr static std::string_view name = get_top_name(#EnumType);      \
        /* A list of all the values in the enum, in declaration order */       \
        constexpr static auto values = std::array {                            \
            MAP_OPERATOR(EnumType, !, __VA_ARGS__)};                           \
        constexpr static auto count = values.size();                           \
                                                                               \
       private:                                                                \
        constexpr static auto names_raw = split_trim<count>(#__VA_ARGS__);     \
                                                                               \
       public:                                                                 \
        constexpr static auto name_lengths = map_array(                        \
            names_raw,                                                         \
            [](std::string_view name) { return name.size(); });                \
                                                                               \
       private:                                                                \
        constexpr static size_t                                                \
            name_block_buffer_size = count                                     \
                                   + fold(                                     \
                                         name_lengths.data(),                  \
                                         count,                                \
                                         0,                                    \
                                         [](size_t acc, size_t elem) {         \
                                             return acc + elem;                \
                                         });                                   \
        constexpr static auto name_block = static_cat<name_block_buffer_size>( \
            names_raw,                                                         \
            '\0');                                                             \
        constexpr static auto lowercase_name_block = map_array(                \
            name_block,                                                        \
            to_lower);                                                         \
                                                                               \
       public:                                                                 \
        /* A list of all the names in the enum, in declaration order */        \
        constexpr static auto names = split_by_lengths_assuming_sep(           \
            name_lengths,                                                      \
            name_block.data());                                                \
        /* All the enum names, but lowercase. Provided to support lookup       \
         * operations that ignore case. */                                     \
        constexpr static auto lowercase_names = split_by_lengths_assuming_sep( \
            name_lengths,                                                      \
            lowercase_name_block.data());                                      \
        constexpr static size_t max_name_length = count == 0                   \
                                                    ? 0                        \
                                                    : *std::max_element(       \
                                                        name_lengths.data(),   \
                                                        name_lengths.data()    \
                                                            + count);          \
        constexpr static base_type min = min_base_value<BaseType>(values);     \
        constexpr static base_type max = max_base_value<BaseType>(values);     \
        constexpr static auto values_to_names = select_map<                    \
            EnumType,                                                          \
            std::string_view,                                                  \
            count,                                                             \
            base_type,                                                         \
            min,                                                               \
            max>(values, names, "<unnamed>");                                  \
        constexpr static auto                                                  \
            names_to_values = binary_map<std::string_view, EnumType, count>(   \
                names,                                                         \
                values);                                                       \
        constexpr static auto lowercase_names_to_values =                      \
            binary_map<std::string_view, EnumType, count>(                     \
                lowercase_names,                                               \
                values);                                                       \
        constexpr static std::optional<EnumType> get_value(                    \
            std::string_view name) {                                           \
            return names_to_values[name];                                      \
        }                                                                      \
        constexpr static EnumType get_value(                                   \
            std::string_view name,                                             \
            EnumType alternative) {                                            \
            return names_to_values.get(name, alternative);                     \
        }                                                                      \
        constexpr static std::optional<EnumType> get_value_ignore_case(        \
            std::string_view name) {                                           \
            if (name.size() > max_name_length) {                               \
                return std::nullopt;                                           \
            } else {                                                           \
                char buffer[max_name_length + 1];                              \
                for (size_t i = 0; i < name.size(); i++) {                     \
                    buffer[i] = to_lower(name[i]);                             \
                }                                                              \
                buffer[names.size()] = '\0';                                   \
                return lowercase_names_to_values[std::string_view(             \
                    buffer,                                                    \
                    names.size())];                                            \
            }                                                                  \
        }                                                                      \
        constexpr static EnumType get_value_ignore_case(                       \
            std::string_view name,                                             \
            EnumType alternative) {                                            \
            if (name.size() > max_name_length) {                               \
                return alternative;                                            \
            } else {                                                           \
                char buffer[max_name_length + 1];                              \
                for (size_t i = 0; i < name.size(); i++) {                     \
                    buffer[i] = to_lower(name[i]);                             \
                }                                                              \
                buffer[names.size()] = '\0';                                   \
                return lowercase_names_to_values.get(                          \
                    std::string_view(buffer, names.size()),                    \
                    alternative);                                              \
            }                                                                  \
        }                                                                      \
        constexpr static std::optional<std::string_view> get_name(             \
            EnumType value) {                                                  \
            return values_to_names[value];                                     \
        }                                                                      \
        constexpr static std::string_view get_name(                            \
            EnumType value,                                                    \
            std::string_view alternative) {                                    \
            return values_to_names.get(value, alternative);                    \
        }                                                                      \
    };                                                                         \
    } // namespace nav::impl
