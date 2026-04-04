#include <gtest/gtest.h>
#include "Order.h"
#include "OrderBook.h"
#include "Utils.h"
#include <chrono>
#include <thread>

TEST(TestOrderBook, AddOrder) {
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

    EXPECT_FALSE(book.isBuySideEmpty());
    EXPECT_FALSE(book.isSellSideEmpty());
    EXPECT_EQ(book.getBuyDepth(), 1);
    EXPECT_EQ(book.getSellDepth(), 1);

    EXPECT_EQ(book.getBestBid(), order1);
    EXPECT_EQ(book.getBestAsk(), order2);

    book.removeBestAsk();
    EXPECT_EQ(book.getBestAsk(), order2);
}

TEST(TestOrderBook, EmptyBook) {
    OrderBook book {OrderBook()};

    EXPECT_TRUE(book.isBuySideEmpty());
    EXPECT_TRUE(book.isSellSideEmpty());
    EXPECT_EQ(book.getBuyDepth(), 0);
    EXPECT_EQ(book.getSellDepth(), 0);

    EXPECT_THROW(book.getBestBid(), std::runtime_error);
    EXPECT_THROW(book.getBestAsk(), std::runtime_error);
}

TEST(TestOrderBook, RemoveOrders) {
    OrderBook book {OrderBook()};

    Order buy {Order(Order::OrderType::buy, 20.0, 1.0, 101, 111)};
    Order sell {Order(Order::OrderType::sell, 20.0, 1.0, 102, 112)};

    book.addOrder(buy);
    book.addOrder(sell);

    book.removeBestBid();
    EXPECT_TRUE(book.isBuySideEmpty());
    EXPECT_FALSE(book.isSellSideEmpty());

    book.removeBestAsk();
    EXPECT_TRUE(book.isBuySideEmpty());
    EXPECT_TRUE(book.isSellSideEmpty());

    EXPECT_THROW(book.removeBestBid(), std::runtime_error);
    EXPECT_THROW(book.removeBestAsk(), std::runtime_error);
}

TEST(TestOrderBook, DepthMultiplePriceLevels) {
    OrderBook book {OrderBook()};

    book.addOrder(Order(Order::OrderType::buy, 20.0, 1.0, 101, 111));
    book.addOrder(Order(Order::OrderType::buy, 25.0, 1.0, 102, 112));

    book.addOrder(Order(Order::OrderType::sell, 30.0, 1.0, 103, 113));
    book.addOrder(Order(Order::OrderType::sell, 35.0, 1.0, 104, 114));
    book.addOrder(Order(Order::OrderType::sell, 40.0, 1.0, 105, 115));

    EXPECT_EQ(book.getBuyDepth(), 2);
    EXPECT_EQ(book.getSellDepth(), 3);
}
