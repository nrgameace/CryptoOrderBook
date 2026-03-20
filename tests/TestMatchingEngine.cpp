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

