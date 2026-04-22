#include "Simulator.h"
#include <thread>
#include "PriceGenerator.h"
#include <vector>
#include "Order.h"

Simulator::Simulator(MatchingEngine& eng, IPriceFetcher& fetcher)
    : engine(eng), priceFetcher(fetcher), running(false) {}

void Simulator::producerLoop() {
    while (running) {
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

void Simulator::consumerLoop() {
    while (running) {
        auto shouldWake = [this] {
            return !transitionQueue.empty() || !running;
        };

        std::unique_lock<std::mutex> lock(mtx);
        empty.wait(lock, shouldWake);

        while (!transitionQueue.empty()) {
            engine.addOrder(transitionQueue.front());
            transitionQueue.pop();
        }

        lock.unlock();
        engine.simulateMarket();
    }
}

MatchingEngine& Simulator::getMatchingEngine() {
    return engine;
}

void Simulator::start() {
    if (!running.exchange(true))
        return;
    producerThread = std::thread(&Simulator::producerLoop, this);
    consumerThread = std::thread(&Simulator::consumerLoop, this);
}

void Simulator::stop() {
    if (!running.exchange(false))
        return;
    empty.notify_all();

    if (producerThread.joinable())
        producerThread.join();
    if (consumerThread.joinable())
        consumerThread.join();
}

Simulator::~Simulator() {
    stop();
}
