#pragma once
#include <nav/nav_core.hpp>
#include <ostream>

namespace nav {
template <nav_enum Enum>
std::ostream& operator<<(std::ostream& os, Enum value) {
    using traits = enum_traits<Enum>;
    if (auto name = traits::get_name(value)) {
        return os << traits::type_name << "::" << name.name();
    } else {
        return os << traits::type_name << "("
                  << typename traits::base_type(value) << ")";
    }
}
} // namespace nav
