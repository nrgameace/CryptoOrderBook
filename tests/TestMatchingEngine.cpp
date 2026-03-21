#include <gtest/gtest.h>
#include "Order.h"
#include "OrderBook.h"
#include "MatchingEngine.h"
#include <chrono>
#include <thread>

TEST(TestMatchingEngine, TestConstructor) {
    Order::OrderType typeBuy = Order::OrderType::buy;
    Order order1 {Order(typeBuy, 20.0, 1.0, 101, 111)};

    Order::OrderType typeSell = Order::OrderType::sell;
    Order order2 {Order(typeSell, 20.0, 1.0, 101, 111)};

    OrderBook book {OrderBook()};

    book.addOrder(order1);
    book.addOrder(order2);

    MatchingEngine engine {MatchingEngine(book)};

    EXPECT_EQ(engine.getBook(), book);
}


TEST(TestMatchingEngine, TestProcessOrder) {
    Order::OrderType typeBuy = Order::OrderType::buy;
    Order order1 {Order(typeBuy, 20.0, 1.0, 101, 111)};
    Order order2 {Order(typeBuy, 25.0, 2.0, 102, 112)};
    Order order6 {Order{typeBuy, 25.0, 1.0, 106, 116}};

    Order::OrderType typeSell = Order::OrderType::sell;
    Order order3 {Order(typeSell, 20.0, 1.5, 103, 113)};
    Order order4 {Order(typeSell, 15.0, 3.0, 104, 114)};
    Order order5 {Order(typeSell, 20.0, 1.0, 105, 115)};

    OrderBook book {OrderBook()};

    book.addOrder(order1);
    book.addOrder(order2);
    book.addOrder(order3);
    book.addOrder(order4);

    MatchingEngine engine {MatchingEngine(book)};
    EXPECT_FALSE(engine.processOrder(order1, order3));
    EXPECT_THROW(engine.processOrder(order1, order2), std::runtime_error);
    EXPECT_TRUE(engine.processOrder(order6, order5));


}