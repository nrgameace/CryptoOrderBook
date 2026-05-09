#pragma once
#include <queue>
#include <thread>
#include "Order.h"
#include "MatchingEngine.h"
#include "IPriceFetcher.h"
#include "MarketStats.h"
#include <mutex>
#include <condition_variable>
#include <chrono>

class Simulator {
    std::queue<Order> transitionQueue;
    MatchingEngine& engine;
    IPriceFetcher& priceFetcher;
    MarketStats stats;
    std::mutex mtx;
    std::condition_variable_any empty;
    std::chrono::system_clock::time_point lastPrint;

    std::jthread producerThread;
    std::jthread consumerThread;

public:
    Simulator(MatchingEngine& eng, IPriceFetcher& fetcher);
    ~Simulator();
    Simulator(const Simulator&) = delete;
    Simulator& operator=(const Simulator&) = delete;
    void producerLoop(std::stop_token st);
    void consumerLoop(std::stop_token st);
    void start();
    void stop();
    MatchingEngine& getMatchingEngine();
};
