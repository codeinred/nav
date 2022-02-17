#pragma once
#include <algorithm>
#include <array>
#include <string_view>
#include <type_traits>

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
template <class Key, class BaseT, class Value, BaseT Min, BaseT Max>
class array_map {
    constexpr static size_t ArraySize = Max - Min + 1;
    std::array<Value, ArraySize> values;
    Value default_value;

   public:
    template <size_t N>
    constexpr array_map(
        std::array<Key, N> const& keys,
        std::array<Value, N> const& values,
        Value default_value)
      : values()
      , default_value(default_value) {
        std::array<bool, ArraySize> visited {};
        for (auto& val : this->values) {
            val = default_value;
        }
        for (size_t i = 0; i < N; i++) {
            auto key_i = BaseT(keys[i]) - Min;
            if (!visited[key_i]) {
                this->values[key_i] = values[i];
                visited[key_i] = true;
            }
        }
    }

    constexpr auto operator[](Key key) const -> Value {
        auto i = BaseT(key);
        if (i < Min || i > Max) {
            return default_value;
        } else {
            return values[i - Min];
        }
    }
};
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
        /* A list of all the names in the enum, in declaration order */        \
        constexpr static auto names = split_trim<count>(#__VA_ARGS__);         \
        constexpr static base_type min = min_base_value<BaseType>(values);     \
        constexpr static base_type max = max_base_value<BaseType>(values);     \
        constexpr static std::string_view get_name(EnumType value) {           \
            constexpr static auto name_map =                                   \
                array_map<EnumType, base_type, std::string_view, min, max>(    \
                    values,                                                    \
                    names,                                                     \
                    "<unnamed>");                                              \
            return name_map[value];                                            \
        }                                                                      \
    };                                                                         \
    } // namespace nav
