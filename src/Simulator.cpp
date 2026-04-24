#include "Simulator.h"
#include <thread>

#include "PriceGenerator.h"
#include <vector>
#include "Order.h"

Simulator::Simulator(MatchingEngine& eng, IPriceFetcher& fetcher)
    : engine(eng), priceFetcher(fetcher) {}

void Simulator::producerLoop(std::stop_token st) {
    while (!st.stop_requested()) {

        PriceGenerator priceGen(priceFetcher);
        priceGen.generateOrders(10);
        std::vector<Order> orders = priceGen.getOrders();

        {
            std::lock_guard<std::mutex> lock(mtx);
            for (Order order : orders) {
                transitionQueue.push(order);
            }
        }
        empty.notify_one();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void Simulator::consumerLoop(std::stop_token st) {
    while (true) {
        std::vector<Order> batch;

        auto shouldWake = [this, st] {
            return !transitionQueue.empty() || !st.stop_requested();
        };

        std::unique_lock<std::mutex> lock(mtx);
        empty.wait(lock, shouldWake);

        if (transitionQueue.empty() && !st.stop_requested())
            return;

        while (!transitionQueue.empty()) {
            batch.push_back(std::move(transitionQueue.front()));
            transitionQueue.pop();
        }

        lock.unlock();

        for (Order& order : batch)
            engine.addOrder(order);

        engine.simulateMarket();


    }
}

MatchingEngine& Simulator::getMatchingEngine() {
    return engine;
}

void Simulator::start() {

    producerThread = std::jthread([this](std::stop_token st){producerLoop(st); });
    consumerThread = std::jthread([this](std::stop_token st){consumerLoop(st); });
}

void Simulator::stop() {
    producerThread.request_stop();
    consumerThread.request_stop();
}

Simulator::~Simulator() {
    stop();
}
