#pragma once
#include <vector>
#include <chrono>
#include <cstdint>

class MarketStats {
public:
    explicit MarketStats(int windowTrades = 100, double windowSeconds = 60.0);
    void recordTrade(int64_t price, int64_t quantity);
    void updateSpread(int64_t bid, int64_t ask);
    void printSummary() const;

    double getVWAP() const;
    double getSpread() const;
    double getVolume() const;
    double getVolatility() const;

private:
    struct TradePoint {
        int64_t price;
        int64_t quantity;
        std::chrono::system_clock::time_point time;
    };

    std::vector<TradePoint> buffer;
    size_t head = 0;
    bool full = false;
    int windowTrades;
    double windowSeconds;
    int64_t lastBid = 0;
    int64_t lastAsk = 0;
};
