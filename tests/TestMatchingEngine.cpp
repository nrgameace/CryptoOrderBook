#include <gtest/gtest.h>
#include "Order.h"
#include "OrderBook.h"
#include "MatchingEngine.h"
#include <chrono>
#include <thread>
#include <iostream>

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
    Order order3 {Order{typeBuy, 25.0, 1.0, 106, 116}};

    Order::OrderType typeSell = Order::OrderType::sell;
    Order order4 {Order(typeSell, 20.0, 1.5, 103, 113)};
    Order order5 {Order(typeSell, 20.0, 1.0, 105, 115)};

    OrderBook book {OrderBook()};


    MatchingEngine engine {MatchingEngine(book)};
    EXPECT_FALSE(engine.processOrder(order1, order4));
    EXPECT_THROW(engine.processOrder(order1, order2), std::runtime_error);
    EXPECT_TRUE(engine.processOrder(order3, order5));


}

TEST(TestMatchingEngine, TestSimulateMarket) {
    Order::OrderType typeBuy = Order::OrderType::buy;
    Order order1 {Order(typeBuy, 20.0, 1.0, 101, 111)};
    Order order2 {Order(typeBuy, 25.0, 2.0, 102, 112)};
    Order order6 {Order{typeBuy, 25.0, 1.0, 106, 116}};

    Order::OrderType typeSell = Order::OrderType::sell;
    Order order3 {Order(typeSell, 20.0, 1.5, 103, 113)};
    Order order4 {Order(typeSell, 15.0, 3.0, 104, 114)};
    Order order5 {Order(typeSell, 20.0, 1.0, 105, 115)};

    OrderBook book1 {OrderBook()};

    book1.addOrder(order1);
    book1.addOrder(order2);
    book1.addOrder(order3);
    book1.addOrder(order4);
    book1.addOrder(order5);
    book1.addOrder(order6);


    MatchingEngine engine1 {MatchingEngine(book1)};

    EXPECT_FALSE(engine1.simulateMarket());


    Order order7 {Order(typeBuy, 20.0, 1.0, 101, 111)};
    Order order8 {Order(typeBuy, 20.0, 2.0, 102, 112)};
    Order order9 {Order{typeBuy, 15.0, 1.0, 106, 116}};

    Order order10 {Order(typeSell, 20.0, 1.0, 103, 113)};
    Order order11 {Order(typeSell, 20.0, 2.0, 104, 114)};
    Order order12 {Order(typeSell, 15.0, 1.0, 105, 115)};

    OrderBook book2 {OrderBook()};

    book2.addOrder(order7);
    book2.addOrder(order8);
    book2.addOrder(order9);
    book2.addOrder(order10);
    book2.addOrder(order11);
    book2.addOrder(order12);




    MatchingEngine engine2 {MatchingEngine(book2)};

    bool val = engine2.simulateMarket();

    std::cout << order7.quantity << std::endl;
    std::cout << order8.quantity << std::endl;
    std::cout << order9.quantity << std::endl;
    std::cout << order10.quantity << std::endl;
    std::cout << order11.quantity << std::endl;
    std::cout << order12.quantity << std::endl;
    std::cout << std::endl;
    std::cout << book2.buyOffers.size() << std::endl;
    std::cout << book2.sellOffers.size() << std::endl;


    EXPECT_TRUE(val);


    Order order13 {Order(typeBuy, 20.0, 1.0, 101, 111)};
    Order order14 {Order(typeBuy, 20.0, 2.0, 102, 112)};
    Order order15 {Order{typeBuy, 15.0, 1.0, 106, 116}};


    OrderBook book3 {OrderBook()};

    book3.addOrder(order13);
    book3.addOrder(order14);
    book3.addOrder(order15);

    MatchingEngine engine3 {MatchingEngine(book3)};

    EXPECT_FALSE(engine3.simulateMarket());




}