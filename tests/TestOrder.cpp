#include <gtest/gtest.h>
#include "Order.h"
#include <chrono>
#include <thread>


TEST(TestOrder, AccessDataFields) {
    Order::OrderType type = Order::OrderType::buy;
    Order order1 {Order(type, 20.0, 1.0, 101, 111)};

    // Values for order1 are chosen at random to ensure all variables are initialized properly.
    EXPECT_EQ(order1.transactionSide, type);
    EXPECT_EQ(order1.price, convertToInternal(20.0));
    EXPECT_EQ(order1.quantity, convertToInternal(1.0));
    EXPECT_EQ(order1.transactionId, 101);
    EXPECT_EQ(order1.userId, 111);
}

TEST(TestOrder, TimestampAccessor) {
    Order::OrderType type = Order::OrderType::buy;

    // These values are chosen at random. The only instance variable of importance is the timestamp.
    Order orderTime {Order(type, 20.0, 1.0, 101, 111)};
    std::chrono::system_clock::time_point time = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed = orderTime.getTimestamp() - time;

    const double EPS = 0.001;
    EXPECT_LE(elapsed.count(), EPS);
}

TEST(TestOrder, LessThanOpperand) {
    // This test has random instance variables but intentionally puts one second in between the timestamps to ensure if the less-than operand is properly implemented.
    Order::OrderType type = Order::OrderType::sell;
    Order order1 {Order(type, 20.0, 1.0, 101, 111)};
    std::this_thread::sleep_for(std::chrono::seconds(1));
    Order order2 {Order(type, 20.0, 1.0, 102, 112)};

    EXPECT_TRUE(order1 < order2);

}