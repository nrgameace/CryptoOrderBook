#include <gtest/gtest.h>
#include "Order.h"
#include "OrderBook.h"
#include "Utils.h"
#include <chrono>
#include <thread>

TEST(TestOrderBook, AddOrder) {
    // Both orders share the same price (20.0) to verify that buy and sell sides are stored separately at the same price level.
    // Other values are chosen at random. order2 is added twice to test that duplicate orders are correctly placed in the same price-level queue.
    Order::OrderType typeBuy = Order::OrderType::buy;
    Order order1 {Order(typeBuy, 20.0, 1.0, 101, 111)};

    Order::OrderType typeSell = Order::OrderType::sell;
    Order order2 {Order(typeSell, 20.0, 1.0, 101, 111)};

    OrderBook book {OrderBook()};

    bool value1 = book.addOrder(order1);
    bool value2 = book.addOrder(order2);
    bool value3 = book.addOrder(order2);

    EXPECT_TRUE(value1);
    EXPECT_TRUE(value2);
    EXPECT_TRUE(value3);

    auto mapBuy = book.buyOffers;
    auto mapSell = book.sellOffers;

    EXPECT_EQ(mapBuy[order1.price].top(), order1);
    EXPECT_EQ(mapSell[order2.price].top(), order2);

    mapSell[order2.price].pop();

    EXPECT_EQ(mapSell[order2.price].top(), order2);




}
