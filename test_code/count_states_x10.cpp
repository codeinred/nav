#include <nav/nav_core.hpp>

#ifndef TEST_VALUES
#define NUM_VALUES 1
#define TEST_VALUES E0
#endif
nav_declare_enum_class(Test0, int, TEST_VALUES);
nav_declare_enum_class(Test1, int, TEST_VALUES);
nav_declare_enum_class(Test2, int, TEST_VALUES);
nav_declare_enum_class(Test3, int, TEST_VALUES);
nav_declare_enum_class(Test4, int, TEST_VALUES);
nav_declare_enum_class(Test5, int, TEST_VALUES);
nav_declare_enum_class(Test6, int, TEST_VALUES);
nav_declare_enum_class(Test7, int, TEST_VALUES);
nav_declare_enum_class(Test8, int, TEST_VALUES);
nav_declare_enum_class(Test9, int, TEST_VALUES);


static_assert(NUM_VALUES == nav::num_states<Test0>);
static_assert(NUM_VALUES == nav::num_states<Test1>);
static_assert(NUM_VALUES == nav::num_states<Test2>);
static_assert(NUM_VALUES == nav::num_states<Test3>);
static_assert(NUM_VALUES == nav::num_states<Test4>);
static_assert(NUM_VALUES == nav::num_states<Test5>);
static_assert(NUM_VALUES == nav::num_states<Test6>);
static_assert(NUM_VALUES == nav::num_states<Test7>);
static_assert(NUM_VALUES == nav::num_states<Test8>);
static_assert(NUM_VALUES == nav::num_states<Test9>);

int main() {}
