#include <gtest/gtest.h>

int add(int x, int y) {
    return x + y;
}


TEST(addTest, PositiveNumbers) {
    ASSERT_EQ(add(3,4), 7);
}