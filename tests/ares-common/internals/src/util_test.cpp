/**
 * @file util_test.cpp
 *
 * @brief
 *
 * @date 5/4/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#include <ares/util.hpp>
#include <gtest/gtest.h>
#include <string>

struct Foo {
    int i1;
    int i2;

    bool operator==(const Foo &other) const {
        return (i1 == other.i1) && (i2 == other.i2);
    }
};

struct TestStruct {
    char c;
    short s;
    int i;
    long l;

    std::string str;

    Foo f;

    bool operator==(const TestStruct &other) const {
        return (c == other.c) && (s == other.s) && (i == other.i) &&
               (l == other.l) && (str == other.str) && (f == other.f);
    }
};

static TestStruct *test_struct_from_c(char *c) {
    return ares::container_of(c, &TestStruct::c);
}

static TestStruct *test_struct_from_s(short *s) {
    return ares::container_of(s, &TestStruct::s);
}

static TestStruct *test_struct_from_i(int *i) {
    return ares::container_of(i, &TestStruct::i);
}

static TestStruct *test_struct_from_l(long *l) {
    return ares::container_of(l, &TestStruct::l);
}

static TestStruct *test_struct_from_str(std::string *str) {
    return ares::container_of(str, &TestStruct::str);
}

static TestStruct *test_struct_from_f(Foo *f) {
    return ares::container_of(f, &TestStruct::f);
}

static Foo *foo_from_i1(int *i1) { return ares::container_of(i1, &Foo::i1); }

static Foo *foo_from_i2(int *i2) { return ares::container_of(i2, &Foo::i2); }

TEST(AresUtils, ContainerOf) {
    TestStruct original{
        .c = 0,
        .s = 1,
        .i = 2,
        .l = 3,
        .str = "bar",
        .f =
            {
                .i1 = 4,
                .i2 = 5,
            },
    };

    TestStruct *ptr = test_struct_from_c(&original.c);
    EXPECT_EQ(original, *ptr);
    ptr->c++;
    EXPECT_EQ(original, *ptr);

    ptr = test_struct_from_s(&original.s);
    EXPECT_EQ(original, *ptr);
    ptr->s++;
    EXPECT_EQ(original, *ptr);

    ptr = test_struct_from_i(&original.i);
    EXPECT_EQ(original, *ptr);
    ptr->i++;
    EXPECT_EQ(original, *ptr);

    ptr = test_struct_from_l(&original.l);
    EXPECT_EQ(original, *ptr);
    ptr->l++;
    EXPECT_EQ(original, *ptr);

    ptr = test_struct_from_str(&original.str);
    EXPECT_EQ(original, *ptr);
    ptr->str = "foo";
    EXPECT_EQ(original, *ptr);

    Foo *f = foo_from_i1(&original.f.i1);
    ptr = test_struct_from_f(f);
    EXPECT_EQ(original, *ptr);
    f->i1++;
    EXPECT_EQ(original, *ptr);

    f = foo_from_i2(&original.f.i2);
    ptr = test_struct_from_f(f);
    EXPECT_EQ(original, *ptr);
    f->i2++;
    EXPECT_EQ(original, *ptr);
}
