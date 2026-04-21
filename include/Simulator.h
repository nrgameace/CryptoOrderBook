#pragma once
#include <queue>
#include "Order.h"
#include "MatchingEngine.h"
#include "IPriceFetcher.h"
#include <mutex>
#include <condition_variable>
#include <atomic>

class Simulator {
    std::queue<Order> transitionQueue;
    MatchingEngine& engine;
    IPriceFetcher& priceFetcher;
    std::mutex mtx;
    std::condition_variable empty;
    std::atomic<bool> running;

public:
    Simulator(MatchingEngine& eng, IPriceFetcher& fetcher);
    void producerLoop();
    void consumerLoop();
    void start();
    void stop();
    MatchingEngine& getMatchingEngine();
};
