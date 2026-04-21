#include <gtest/gtest.h>
#include "Simulator.h"
#include "OrderBook.h"
#include "MatchingEngine.h"
#include "MockTransactionLogger.h"
#include "IPriceFetcher.h"
#include <thread>
#include <chrono>

namespace {
class MockPriceFetcher : public IPriceFetcher {
    double price;
public:
    MockPriceFetcher(double price) : price(price) {}
    double fetchPrice() override { return price; }
};
}

TEST(TestSimulator, TestConstructor) {
    OrderBook book;
    MockTransactionLogger logTemp;
    TransactionLoggerInterface& log = logTemp;
    MatchingEngine engine(book, log);
    MockPriceFetcher fetcher(50000.0);

    Simulator sim(engine, fetcher);
    EXPECT_EQ(&sim.getMatchingEngine(), &engine);
}

TEST(TestSimulator, TestStartStop) {
    OrderBook book;
    MockTransactionLogger logTemp;
    TransactionLoggerInterface& log = logTemp;
    MatchingEngine engine(book, log);
    MockPriceFetcher fetcher(50000.0);

    Simulator sim(engine, fetcher);

    std::thread stopper([&] {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        sim.stop();
    });

    sim.start();
    stopper.join();

    EXPECT_TRUE(true);
}
