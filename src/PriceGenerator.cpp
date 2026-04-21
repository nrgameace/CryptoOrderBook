#include "PriceGenerator.h"
#include <random>

PriceGenerator::PriceGenerator(IPriceFetcher& fetcher)
    : fetcher(fetcher), currentPrice(0.0), orders() {}

void PriceGenerator::updatePrice() {
    currentPrice = fetcher.fetchPrice();
}

std::vector<Order> PriceGenerator::getOrders() {
    return orders;
}

void PriceGenerator::generateOrders(int numOrders) {
    std::random_device seed;
    updatePrice();

    std::normal_distribution<double> priceDist(currentPrice, 15);
    std::uniform_int_distribution<int> sideDist{0, 1};

    orders.clear();

    for (int i {}; i < numOrders; i++) {
        int priceTemp = priceDist(seed);
        int randomSide = sideDist(seed);

        Order::OrderType side = (randomSide == 1) ? Order::OrderType::sell : Order::OrderType::buy;
        orders.push_back(Order(side, priceTemp, 2.0, 1, 1));
    }
}
