#pragma once
#include <queue>
#include <thread>
#include "Order.h"
#include "MatchingEngine.h"
#include "IPriceFetcher.h"
#include <mutex>
#include <condition_variable>

class Simulator {
    std::queue<Order> transitionQueue;
    MatchingEngine& engine;
    IPriceFetcher& priceFetcher;
    std::mutex mtx;
    std::condition_variable_any empty;

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
