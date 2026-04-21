#include <gtest/gtest.h>
#include "Utils.h"

TEST(TestUtils, ZeroConversion) {
    EXPECT_EQ(convertToInternal(0.0), 0);
    EXPECT_DOUBLE_EQ(convertToDouble(0), 0.0);
}

TEST(TestUtils, IntegerConversion) {
    EXPECT_EQ(convertToInternal(1.0), SCALE_FACTOR);
    EXPECT_DOUBLE_EQ(convertToDouble(SCALE_FACTOR), 1.0);
}

TEST(TestUtils, FractionalConversion) {
    EXPECT_EQ(convertToInternal(0.5), SCALE_FACTOR / 2);
    EXPECT_DOUBLE_EQ(convertToDouble(SCALE_FACTOR / 2), 0.5);
}

TEST(TestUtils, RoundTrip) {
    double value = 123.456789;
    EXPECT_NEAR(convertToDouble(convertToInternal(value)), value, 1e-6);
}

TEST(TestUtils, NegativeValues) {
    EXPECT_EQ(convertToInternal(-1.0), -SCALE_FACTOR);
    EXPECT_DOUBLE_EQ(convertToDouble(-SCALE_FACTOR), -1.0);
}

TEST(TestUtils, Precision) {
    EXPECT_EQ(convertToInternal(1.12345678), 112345678);
}
