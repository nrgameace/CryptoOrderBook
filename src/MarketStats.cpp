#include "MarketStats.h"
#include "Utils.h"
#include <cmath>
#include <iostream>
#include <iomanip>

MarketStats::MarketStats(int windowTrades, double windowSeconds)
    : windowTrades(windowTrades), windowSeconds(windowSeconds) {
    buffer.resize(windowTrades);
}

void MarketStats::recordTrade(int64_t price, int64_t quantity) {
    buffer[head] = { price, quantity, std::chrono::system_clock::now() };
    head = (head + 1) % windowTrades;
    if (!full && head == 0) full = true;
}

void MarketStats::updateSpread(int64_t bid, int64_t ask) {
    lastBid = bid;
    lastAsk = ask;
}

double MarketStats::getVWAP() const {
    double sumPQ = 0.0, sumQ = 0.0;
    int count = full ? windowTrades : static_cast<int>(head);
    int start = full ? static_cast<int>(head) : 0;
    for (int i = 0; i < count; i++) {
        const TradePoint& t = buffer[(start + i) % windowTrades];
        double p = convertToDouble(t.price);
        double q = convertToDouble(t.quantity);
        sumPQ += p * q;
        sumQ  += q;
    }
    return sumQ > 0.0 ? sumPQ / sumQ : 0.0;
}

double MarketStats::getSpread() const {
    if (lastBid == 0 || lastAsk == 0) return 0.0;
    return convertToDouble(lastAsk - lastBid);
}

double MarketStats::getVolume() const {
    auto now = std::chrono::system_clock::now();
    int count = full ? windowTrades : static_cast<int>(head);
    int start = full ? static_cast<int>(head) : 0;
    double sumQ = 0.0;
    for (int i = 0; i < count; i++) {
        const TradePoint& t = buffer[(start + i) % windowTrades];
        double age = std::chrono::duration<double>(now - t.time).count();
        if (age < windowSeconds) sumQ += convertToDouble(t.quantity);
    }
    return sumQ;
}

double MarketStats::getVolatility() const {
    int count = full ? windowTrades : static_cast<int>(head);
    int start = full ? static_cast<int>(head) : 0;
    if (count < 2) return 0.0;
    double mean = 0.0, M2 = 0.0;
    for (int i = 0; i < count; i++) {
        double p = convertToDouble(buffer[(start + i) % windowTrades].price);
        double delta = p - mean;
        mean += delta / (i + 1);
        M2 += delta * (p - mean);
    }
    return std::sqrt(M2 / (count - 1));
}

void MarketStats::printSummary() const {
    std::cout << std::fixed << std::setprecision(2)
              << "=== Market Summary ===\n"
              << "VWAP:        $" << getVWAP()       << "\n"
              << "Spread:      $" << getSpread()      << "\n"
              << "Volume:       " << getVolume()      << "\n"
              << "Volatility:  $" << getVolatility()  << "\n";
}
