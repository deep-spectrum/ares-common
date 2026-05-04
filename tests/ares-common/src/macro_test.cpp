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
