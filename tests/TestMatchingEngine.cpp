#include <gtest/gtest.h>
#include "Order.h"
#include "OrderBook.h"
#include "MatchingEngine.h"
#include "MockTransactionLogger.h"
#include <chrono>
#include <thread>

TEST(TestMatchingEngine, TestConstructor) {
    // Values are chosen at random. Only one buy and one sell order are needed to verify the engine holds a correct reference to the order book.
    Order::OrderType typeBuy = Order::OrderType::buy;
    Order order1 {Order(typeBuy, 20.0, 1.0, 101, 111)};

    Order::OrderType typeSell = Order::OrderType::sell;
    Order order2 {Order(typeSell, 20.0, 1.0, 101, 111)};

    OrderBook book {OrderBook()};

    book.addOrder(order1);
    book.addOrder(order2);

    MockTransactionLogger logTemp {MockTransactionLogger()};
    TransactionLoggerInterface& log1 = logTemp;
    MatchingEngine engine {MatchingEngine(book, log1)};

    EXPECT_EQ(engine.getBook(), book);
}


TEST(TestMatchingEngine, TestProcessOrder) {
    // order1 has qty 1.0 to create a partial fill against order4's qty 1.5. order2 is a second buy order used to test that passing two buy-side orders throws an exception.
    // order3 has qty 1.0 matching order5's qty 1.0 to test a perfect fill. Other values are chosen at random.
    Order::OrderType typeBuy = Order::OrderType::buy;
    Order order1 {Order(typeBuy, 20.0, 1.0, 101, 111)};
    Order order2 {Order(typeBuy, 25.0, 2.0, 102, 112)};
    Order order3 {Order{typeBuy, 25.0, 1.0, 106, 116}};

    // order4 has qty 1.5 (greater than order1's 1.0) to produce a partial fill. order5 has qty 1.0 matching order3 for a perfect fill. Prices are at 20.0 to ensure valid matching.
    Order::OrderType typeSell = Order::OrderType::sell;
    Order order4 {Order(typeSell, 20.0, 1.5, 103, 113)};
    Order order5 {Order(typeSell, 20.0, 1.0, 105, 115)};

    OrderBook book {OrderBook()};
    MockTransactionLogger logTemp {MockTransactionLogger()};
    TransactionLoggerInterface& log2 = logTemp;

    MatchingEngine engine {MatchingEngine(book, log2)};
    EXPECT_FALSE(engine.processOrder(order1, order4));
    EXPECT_THROW(engine.processOrder(order1, order2), std::runtime_error);
    EXPECT_TRUE(engine.processOrder(order3, order5));


}

TEST(TestMatchingEngine, TestSimulateMarket) {

    // Total buy qty (4.0) intentionally differs from total sell qty (5.5) so the market cannot fully clear.
    // Prices overlap to ensure orders can match. Other values are chosen at random.
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

    MockTransactionLogger logTemp {MockTransactionLogger()};
    TransactionLoggerInterface& log3 = logTemp;
    MatchingEngine engine1 {MatchingEngine(book1, log3)};

    EXPECT_FALSE(engine1.simulateMarket());
    EXPECT_FALSE(book1.isBuySideEmpty() && book1.isSellSideEmpty());

    // Total buy qty (4.0) equals total sell qty (4.0) and all buy prices are >= sell prices, so the market fully clears.
    // Other values are chosen at random.
    Order order7 {Order(typeBuy, 20.0, 1.0, 101, 111)};
    Order order8 {Order(typeBuy, 20.0, 2.0, 102, 112)};
    Order order9 {Order{typeBuy, 25.0, 1.0, 106, 116}};

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

    MockTransactionLogger logTemp1 {MockTransactionLogger()};
    TransactionLoggerInterface& log4 = logTemp1;
    MatchingEngine engine2 {MatchingEngine(book2, log4)};

    EXPECT_TRUE(engine2.simulateMarket());
    EXPECT_TRUE(book2.isBuySideEmpty());
    EXPECT_TRUE(book2.isSellSideEmpty());
    EXPECT_EQ(book2.getBuyDepth(), 0);
    EXPECT_EQ(book2.getSellDepth(), 0);

    // Only buy-side orders are added to test that the method returns false when one side of the book is empty. Values are chosen at random.
    Order order13 {Order(typeBuy, 20.0, 1.0, 101, 111)};
    Order order14 {Order(typeBuy, 20.0, 2.0, 102, 112)};
    Order order15 {Order{typeBuy, 15.0, 1.0, 106, 116}};


    OrderBook book3 {OrderBook()};

    book3.addOrder(order13);
    book3.addOrder(order14);
    book3.addOrder(order15);

    MockTransactionLogger logTemp2 {MockTransactionLogger()};
    TransactionLoggerInterface& log5 = logTemp2;
    MatchingEngine engine3 {MatchingEngine(book3, log5)};

    EXPECT_FALSE(engine3.simulateMarket());
    EXPECT_FALSE(book3.isBuySideEmpty());
    EXPECT_TRUE(book3.isSellSideEmpty());
    EXPECT_EQ(book3.getBuyDepth(), 2);

    // Total quantities match (4.0 each) but the buy at 15.0 cannot match the sell at 20.0, leaving unmatched orders.
    // This tests that the spread check prevents invalid matches. Other values are chosen at random.
    Order order16 {Order(typeBuy, 20.0, 1.0, 101, 111)};
    Order order17 {Order(typeBuy, 20.0, 2.0, 102, 112)};
    Order order18 {Order{typeBuy, 15.0, 1.0, 106, 116}};

    Order order19 {Order(typeSell, 20.0, 1.0, 103, 113)};
    Order order20 {Order(typeSell, 20.0, 2.0, 104, 114)};
    Order order21 {Order(typeSell, 15.0, 1.0, 105, 115)};

    OrderBook book4 {OrderBook()};

    book4.addOrder(order16);
    book4.addOrder(order17);
    book4.addOrder(order18);
    book4.addOrder(order19);
    book4.addOrder(order20);
    book4.addOrder(order21);

    MockTransactionLogger logTemp3 {MockTransactionLogger()};
    TransactionLoggerInterface& log6 = logTemp3;
    MatchingEngine engine4 {MatchingEngine(book4, log6)};

    EXPECT_FALSE(engine4.simulateMarket());
    EXPECT_FALSE(book4.isBuySideEmpty());
    EXPECT_FALSE(book4.isSellSideEmpty());
}

