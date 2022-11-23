# NAV: Reflection on Enumeration Names and Values

## Checklist

- [x] Implement `NAV_DECLARE_ENUM`, `NAV_DECLARE_ENUM_CLASS` and `NAV_DECLARE_ENUM_STRUCT` without recursive macro instantiation
- [x] Remove limit on number of elements in an enumeration
- [x] Support enumerations with non-trivial dependencies (these are enumerations
      whose values depend on other members of the enumeration)
- [x] Support enumerations with arbitrarily large values, up to the numeric
      limits of the underlying integral type
- [x] Avoid copying or returning large arrays by value (this speeds compile
      times)
- [x] Avoid expensive type deductions (deducing a std::array type on a large
      number of values proved to be rather expensive)
- [x] Make `nav::enum_names` and `nav::enum_values` indexable and iterable
- [x] Make `nav::enum_names` and `nav::enum_values` compile time constants
- [x] Store enumeration names in a contiguous block of memory to ensure good
      cache locality
- [x] Provide option to null-terminate names (pass
      `-DNAV_ADD_NULL_TERMINATORS=1` to add null terminators after enum names)
- [x] Provide lowercase names for enum values (useful when writing CLIs and
      other applications that expose enumeration names to a user)
- [x] Handle unknown enum types gracefully
- [x] Handle compile-time string operations quickly and consistently via
      `nav::string_block`. This type represents a list of strings, backed
      together at compile time. It's iterable and provides random-access lookup
      of strings, and all strings are stored as a single contiguous block of
      memory, with (optional) null terminators between strings. Indexing into
      the block returns a string_view, and iterators for `nav::string_block`
      similarly return `string_view`s when dereferenced.
- [ ] Get the minimum and maximum value in an enum
- [ ] Look up names from enumeration values
- [ ] Look up enumeration values from names
- [ ] Provide fast O(1) name lookup for compact enumerations (enumerations whose
      values fit in a reasonably small range)
- [ ] Look up lowercase names from values
- [ ] Look up values from lowercase names
- [ ] Provide functions for fuzzy matching on enum names. This is useful for
      reporting errors to a user. You can provide suggestions, like "You put X.
      Did you mean Y?"
- [ ] Provide charconv conversion functions
- [ ] Provide optional integration with libfmt (`#include <nav/nav_libfmt.hpp>`)
- [ ] Provide optional integration for printing to ostreams
      (`#include <nav/nav_iostream.hpp>`)
- [ ] Benchmark compile times against other well-established enumeration
      reflection libraries
- [ ] Benchmark runtime performance against enum reflection in other languages
      (Java, Rust)

## Graceful handling of unknown enumerations

Nav handles unknown enumerations gracefully. `is_nav_enum<EnumType>` is false
for unknown enumerations, the number of states will be reported as 0, and the
number of names and values will be reported as 0 too. The type name and
qualified type name for the enumeration will be deduced via
`nav::get_name_of_type()`, which is a generic function that can get the name of
most types.

```cpp
// Snapshot of test code taken from tests/nav_core.cpp
enum class UnknownEnum { A, B, C };

CHECK(nav::enum_type_info<UnknownEnum>::type_name == "UnknownEnum");
REQUIRE_FALSE(nav::is_nav_enum<UnknownEnum>);
REQUIRE(nav::enum_names<UnknownEnum>.size() == 0);
REQUIRE(nav::enum_names<UnknownEnum>.begin() == nav::enum_names<UnknownEnum>.end());

REQUIRE(nav::enum_values<UnknownEnum>.size() == 0);
REQUIRE(nav::enum_values<UnknownEnum>.begin() == nav::enum_values<UnknownEnum>.end());
```

## Nav supports non-trivial enumerations

Here, some members of the RainbowColors enumeration are declared in terms of
preceeding members. For example, Green is defined in terms of Yellow and Red

```cpp

nav_declare_enum_class(
    RainbowColors,
    int,
    Red = 0xff0000,
    Orange = (Red + 0xffff00) / 2 & 0xffff00,
    Yellow = 0xffff00,
    Green = Yellow - Red,
    Blue = 0x0000ff,
    Indigo = 0x8A2BE2,
    Violet = Red + Blue);
```

## Comparisons against other libraries

|                             | **[nav]** | [magic_enum] | [better-enums] | [enum.hpp] | [wise_enum] |
| --------------------------- | --------- | ------------ | -------------- | ---------- | ----------- |
| Requires macro              | Yes       | No           | Yes            | Yes        | Yes         |
| Limits on min/max value?    | No        | Yes          |                |            |             |
| Limits on number of values? | No        | Yes          |                |            |             |
| Supports aliased values?    | Yes       | No           |                |            |             |
| Supports using enum?        | Yes       | Yes          | No             |            | Yes         |
| Special Declaration Syntax? | No        | No           |                |            | Yes         |
| Compact Names in Binary?    | Yes       |              | No             |            |             |
| Fuzzy Matching Support      | Yes       | No           |                |            |             |
| Lowercase name lookup       | Yes       | Yes          |                |            |             |
| libfmt support              | Yes       |              |                |            |             |
| charconv support            | Yes       |              |                |            |             |

[nav]: https://github.com/codeinred/nav
[magic_enum]: https://github.com/Neargye/magic_enum
[better-enums]: https://github.com/aantron/better-enums
[enum.hpp]: https://github.com/BlackMATov/enum.hpp
[wise_enum]: https://github.com/quicknir/wise_enum
