#pragma once
#include <cmath>
#include <fmt/core.h>
#include <fmt/format.h>
#include <nav/nav_core.hpp>
#include <type_traits>

template <nav::nav_enum Enum>
struct fmt::formatter<Enum> : private fmt::formatter<std::string_view> {
   private:
    using super = fmt::formatter<std::string_view>;

    enum Presentation : char {
        // Just the name of the value
        Name,
        // Name of the value, with enum type attached
        QualifiedName,
        // Name of value with enum type + namespace attached
        FullName,
    };
    // Presentation format: 'f' - fixed, 'e' - exponential.
    Presentation enumFmt = Name;

   public:
    // Parses format specifications of the form ['f' | 'e'].
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        // [ctx.begin(), ctx.end()) is a character range that contains a part of
        // the format string starting from the format specifications to be
        // parsed, e.g. in
        //
        //   fmt::format("{:f} - point of interest", point{1, 2});
        //
        // the range will contain "f} - point of interest". The formatter should
        // parse specifiers until '}' or the end of the range. In this example
        // the formatter should parse the 'f' specifier and return an iterator
        // pointing to '}'.

        // Parse the presentation format and store it in the formatter:
        auto it = ctx.begin(), end = ctx.end();
        if (it != end) {
            switch (*it) {
                case 'N':
                case 'n':
                    enumFmt = Name;
                    it++;
                    break;
                case 'Q':
                case 'q':
                    enumFmt = QualifiedName;
                    it++;
                    break;
                case 'F':
                case 'f':
                    enumFmt = FullName;
                    it++;
                    break;
                default: break;
            }
            ctx.advance_to(it);
        }

        // Return an iterator past the end of the parsed range:
        return super::parse(ctx);
    }

    // Formats the point p using the parsed format specification (presentation)
    // stored in this formatter.
    template <typename FormatContext>
    auto format(const Enum& value, FormatContext& ctx) -> decltype(ctx.out()) {
        using traits = nav::enum_traits<Enum>;
        using base_type = typename traits::base_type;
        switch (enumFmt) {
            default: // If the presentation format is invalid, use this
            case Name: {
                if (auto name = traits::get_name(value)) {
                    return super::format(name.name(), ctx);
                } else {
                    char buffer[traits::type_name.size() + 32] {};
                    auto base_val = base_type(value);

                    char* end = fmt::format_to(
                        buffer,
                        "{}({})",
                        traits::type_name,
                        base_val);
                    return super::format(std::string_view(buffer, end), ctx);
                }
            } break;
            case QualifiedName: {
                constexpr size_t buff_size = traits::type_name.size()
                                           + nav::impl::max(
                                                 size_t(32),
                                                 traits::max_name_length + 2);
                char buffer[buff_size] {};
                char* end;
                if (auto name = traits::get_name(value)) {
                    end = fmt::format_to(
                        buffer,
                        "{}::{}",
                        traits::type_name,
                        name.name());
                } else {
                    auto base_val = base_type(value);

                    end = fmt::format_to(
                        buffer,
                        "{}({})",
                        traits::type_name,
                        base_val);
                }
                return super::format(std::string_view(buffer, end), ctx);
            } break;
            case FullName: {
                constexpr size_t buff_size = traits::qualified_type_name.size()
                                           + nav::impl::max(
                                                 size_t(32),
                                                 traits::max_name_length + 2);
                char buffer[buff_size] {};
                char* end;
                if (auto name = traits::get_name(value)) {
                    end = fmt::format_to(
                        buffer,
                        "{}::{}",
                        traits::qualified_type_name,
                        name.name());
                } else {
                    auto base_val = base_type(value);

                    end = fmt::format_to(
                        buffer,
                        "{}({})",
                        traits::qualified_type_name,
                        base_val);
                }
                return super::format(std::string_view(buffer, end), ctx);
            } break;
        }
    }
};
