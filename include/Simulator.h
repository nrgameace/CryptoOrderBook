#pragma once
#include <queue>
#include <thread>
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

    std::thread producerThread;
    std::thread consumerThread;

public:
    Simulator(MatchingEngine& eng, IPriceFetcher& fetcher);
    ~Simulator();
    Simulator(const Simulator&) = delete;
    Simulator& operator=(const Simulator&) = delete;
    void producerLoop();
    void consumerLoop();
    void start();
    void stop();
    MatchingEngine& getMatchingEngine();
};
