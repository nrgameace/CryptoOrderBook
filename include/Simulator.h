#pragma once
#include <queue>
#include "Order.h"
#include "MatchingEngine.h"
#include <mutex>
#include <condition_variable>
#include <atomic>


class Simulator {
    std::queue<Order> transitionQueue;
    MatchingEngine engine;
    std::mutex mtx;
    std::condition_variable empty;
    std::atomic<bool> running;

    public:
        Simulator();
        void producerLoop();
        void consumerLoop();
        void start();
        void stop();
        MatchingEngine& getMatchingEngine();


};