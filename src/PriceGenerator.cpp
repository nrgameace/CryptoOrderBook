#include "PriceGenerator.h"
#include <random>

PriceGenerator::PriceGenerator(IPriceFetcher& fetcher)
    : fetcher(fetcher), currentPrice(0.0), orders(), rng(std::random_device{}()) {}

void PriceGenerator::updatePrice() {
    currentPrice = fetcher.fetchPrice();
}

std::vector<Order> PriceGenerator::getOrders() {
    return orders;
}

void PriceGenerator::generateOrders(int numOrders) {
    updatePrice();

    std::normal_distribution<double> priceDist(currentPrice, 15);
    std::uniform_int_distribution<int> sideDist{0, 1};

    orders.clear();

    for (int i {}; i < numOrders; i++) {
        double priceTemp = priceDist(rng);
        int randomSide = sideDist(rng);

        Order::OrderType side = (randomSide == 1) ? Order::OrderType::sell : Order::OrderType::buy;
        orders.push_back(Order(side, priceTemp, 2.0, 1, 1));
    }
}
