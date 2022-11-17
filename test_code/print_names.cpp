#include <cstdio>
#include <nav/nav_core.hpp>

#ifndef TEST_VALUES
#define NUM_VALUES 1
#define TEST_VALUES E0
#endif
nav_declare_enum_class(TestEnum, int, TEST_VALUES);

int main() {
    for (std::string_view name : nav::enum_names<TestEnum>) {
        #if NAV_ADD_NULL_TERMINATORS
        printf("%s\n", name.data());
        #else
        printf("%.*s\n", int(name.size()), name.data());
        #endif
    }
}
