#include <gtest/gtest.h>
#include "MarketStats.h"
#include "Utils.h"

TEST(TestMarketStats, TestInvalidWindowTrades) {
    EXPECT_THROW(MarketStats(0), std::invalid_argument);
    EXPECT_THROW(MarketStats(-1), std::invalid_argument);
}

TEST(TestMarketStats, TestEmptyStats) {
    // No trades recorded — all stats should return 0.
    MarketStats stats;
    EXPECT_DOUBLE_EQ(stats.getVWAP(), 0.0);
    EXPECT_DOUBLE_EQ(stats.getSpread(), 0.0);
    EXPECT_DOUBLE_EQ(stats.getVolume(), 0.0);
    EXPECT_DOUBLE_EQ(stats.getVolatility(), 0.0);
}

TEST(TestMarketStats, TestVWAP) {
    // trade1: price=100, qty=2  → contribution 200
    // trade2: price=200, qty=1  → contribution 200
    // VWAP = (200 + 200) / (2 + 1) = 400 / 3 ≈ 133.3333
    MarketStats stats;
    stats.recordTrade(convertToInternal(100.0), convertToInternal(2.0));
    stats.recordTrade(convertToInternal(200.0), convertToInternal(1.0));
    EXPECT_NEAR(stats.getVWAP(), 400.0 / 3.0, 1e-4);
}

TEST(TestMarketStats, TestVolume) {
    // trade1: qty=2, trade2: qty=3 — both within the default 60-second window
    // volume = 2 + 3 = 5.0
    MarketStats stats;
    stats.recordTrade(convertToInternal(100.0), convertToInternal(2.0));
    stats.recordTrade(convertToInternal(200.0), convertToInternal(3.0));
    EXPECT_NEAR(stats.getVolume(), 5.0, 1e-4);
}

TEST(TestMarketStats, TestVolatility) {
    // prices = [10, 20, 30], mean = 20
    // M2 = (10-20)^2 + (20-20)^2 + (30-20)^2 = 100 + 0 + 100 = 200
    // std dev = sqrt(200 / 2) = sqrt(100) = 10.0
    MarketStats stats;
    stats.recordTrade(convertToInternal(10.0), convertToInternal(1.0));
    stats.recordTrade(convertToInternal(20.0), convertToInternal(1.0));
    stats.recordTrade(convertToInternal(30.0), convertToInternal(1.0));
    EXPECT_NEAR(stats.getVolatility(), 10.0, 1e-4);
}

TEST(TestMarketStats, TestSpread) {
    // bid=100, ask=105 → spread = 5.0
    MarketStats stats;
    stats.updateSpread(convertToInternal(100.0), convertToInternal(105.0));
    EXPECT_NEAR(stats.getSpread(), 5.0, 1e-4);
}

TEST(TestMarketStats, TestWindowEviction) {
    // windowTrades=2: after 3 inserts the oldest (price=100) is evicted.
    // Remaining: price=200 qty=1, price=300 qty=1
    // VWAP = (200*1 + 300*1) / (1+1) = 500 / 2 = 250.0
    MarketStats stats(2);
    stats.recordTrade(convertToInternal(100.0), convertToInternal(1.0));
    stats.recordTrade(convertToInternal(200.0), convertToInternal(1.0));
    stats.recordTrade(convertToInternal(300.0), convertToInternal(1.0));
    EXPECT_NEAR(stats.getVWAP(), 250.0, 1e-4);
}
