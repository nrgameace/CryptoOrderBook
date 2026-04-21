#include "Simulator.h"
#include <thread>
#include "PriceGenerator.h"
#include <vector>
#include "Order.h"
#include "OrderBook.h"

Simulator::Simulator(MatchingEngine& eng) 
    : engine(eng), running(false) {}


void Simulator::producerLoop() {
    
    while (running) {
        PriceGenerator priceGen {PriceGenerator()};
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
            bool hasOrders = !transitionQueue.empty();
            bool isStopping = !running;
            return hasOrders || isStopping;
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
    running = true;
    std::thread Consumer(&Simulator::consumerLoop, this);
    std::thread Producer(&Simulator::producerLoop, this);

    Consumer.join();
    Producer.join();
}

void Simulator::stop() {
    running = false;
    empty.notify_all();
}