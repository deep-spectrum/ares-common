/**
 * @file macro_test.cpp
 *
 * @brief Tests for the macros.
 *
 * @date 5/4/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#include <gtest/gtest.h>
#include <ares/util.h>

#define FOO 17
#define BAR 23

TEST(AresUtils, Stringify) {
    EXPECT_STREQ(STRINGIFY(FOO), "FOO");
    EXPECT_STREQ(MACRO_STRINGIFY(BAR), "23");
}

#define FLAG_0 0
#define FLAG_1 1

TEST(AresUtils, ConditionCodeMacros) {
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
