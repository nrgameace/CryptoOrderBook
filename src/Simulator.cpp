#include "Simulator.h"
#include <thread>
#include "PriceGenerator.h"
#include <vector>
#include "Order.h"

Simulator::Simulator(MatchingEngine& eng) 
    : engine(eng) {}


void Simulator::producerLoop() {
    PriceGenerator priceGen {PriceGenerator()};
    while (running) {
        priceGen.generateOrders(10);
        std::vector<Order> orders = priceGen.getOrders();

        {
            std::lock_guard<std::mutex> lock(mtx);
            for (Order order : orders) {
                transitionQueue.push(order);
            }
        }
        empty.notify_one();



    }
}

void Simulator::consumerLoop() {

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