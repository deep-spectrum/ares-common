/**
 * @file macro_test.cpp
 *
 * @brief Tests for the macros.
 *
 * @date 5/4/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#include <ares/util.h>
#include <gtest/gtest.h>

#define FOO 17
#define BAR 23

TEST(AresUtils, Stringify) {
    EXPECT_STREQ(STRINGIFY(FOO), "FOO");
    EXPECT_STREQ(MACRO_STRINGIFY(BAR), "23");
}

TEST(AresUtils, ConditionCodeMacros) {
#define FLAG_0 0
#define FLAG_1 1
    int val;

    COND_CODE_0(FLAG_0, (val = FOO), (val = BAR));
    EXPECT_EQ(val, FOO);

    COND_CODE_0(FLAG_1, (val = FOO), (val = BAR));
    EXPECT_EQ(val, BAR);

    COND_CODE_1(FLAG_0, (val = FOO), (val = BAR));
    EXPECT_EQ(val, BAR);

    COND_CODE_1(FLAG_1, (val = FOO), (val = BAR));
    EXPECT_EQ(val, FOO);
}

TEST(AresUtils, IsEmpty) {
#define test_IS_EMPTY_REAL_EMPTY
#define test_IS_EMPTY_NOT_EMPTY XXX_DO_NOT_REPLACE_XXX
    int foo = 0;
    EXPECT_TRUE(IS_EMPTY(test_IS_EMPTY_REAL_EMPTY));
    EXPECT_FALSE(IS_EMPTY(test_IS_EMPTY_NOT_EMPTY));
    EXPECT_FALSE(IS_EMPTY("string"));
    EXPECT_FALSE(IS_EMPTY(&foo));
    ARG_UNUSED(foo);
}
