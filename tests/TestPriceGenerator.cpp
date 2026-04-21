#include <gtest/gtest.h>
#include "PriceGenerator.h"
#include "IPriceFetcher.h"
#include "Utils.h"

namespace {
class MockPriceFetcher : public IPriceFetcher {
    double price;
public:
    int callCount = 0;
    MockPriceFetcher(double price) : price(price) {}
    double fetchPrice() override {
        callCount++;
        return price;
    }
};
}

TEST(TestPriceGenerator, TestConstructorState) {
    MockPriceFetcher fetcher(50000.0);
    PriceGenerator gen(fetcher);
    EXPECT_TRUE(gen.getOrders().empty());
}

TEST(TestPriceGenerator, TestGenerateOrdersCount) {
    MockPriceFetcher fetcher(50000.0);
    PriceGenerator gen(fetcher);
    gen.generateOrders(5);
    EXPECT_EQ(static_cast<int>(gen.getOrders().size()), 5);
}

TEST(TestPriceGenerator, TestOrderSides) {
    MockPriceFetcher fetcher(50000.0);
    PriceGenerator gen(fetcher);
    gen.generateOrders(20);
    for (const Order& order : gen.getOrders()) {
        bool validSide = order.transactionSide == Order::OrderType::buy ||
                         order.transactionSide == Order::OrderType::sell;
        EXPECT_TRUE(validSide);
    }
}

TEST(TestPriceGenerator, TestOrderQuantity) {
    MockPriceFetcher fetcher(50000.0);
    PriceGenerator gen(fetcher);
    gen.generateOrders(10);
    for (const Order& order : gen.getOrders()) {
        EXPECT_EQ(order.quantity, convertToInternal(2.0));
    }
}

TEST(TestPriceGenerator, TestFetcherCalledOnGenerate) {
    MockPriceFetcher fetcher(50000.0);
    PriceGenerator gen(fetcher);
    gen.generateOrders(5);
    EXPECT_EQ(fetcher.callCount, 1);
}

TEST(TestPriceGenerator, TestFetcherCalledEachGenerate) {
    MockPriceFetcher fetcher(50000.0);
    PriceGenerator gen(fetcher);
    gen.generateOrders(5);
    gen.generateOrders(5);
    EXPECT_EQ(fetcher.callCount, 2);
}

TEST(TestPriceGenerator, TestRegenerateClears) {
    MockPriceFetcher fetcher(50000.0);
    PriceGenerator gen(fetcher);
    gen.generateOrders(5);
    gen.generateOrders(3);
    EXPECT_EQ(static_cast<int>(gen.getOrders().size()), 3);
}

TEST(TestPriceGenerator, TestZeroOrders) {
    MockPriceFetcher fetcher(50000.0);
    PriceGenerator gen(fetcher);
    gen.generateOrders(0);
    EXPECT_TRUE(gen.getOrders().empty());
}
