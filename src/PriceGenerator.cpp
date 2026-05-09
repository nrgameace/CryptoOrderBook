#include "PriceGenerator.h"
#include <random>
#include <cmath>
#include <algorithm>

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
    // Mean and std dev derived from real BTC spot trade data (avg trade size ~0.3228 BTC, variance ~0.15)
    std::normal_distribution<double> qtyDist(0.3228, std::sqrt(0.15));

    orders.clear();

    for (int i {}; i < numOrders; i++) {
        double priceTemp = priceDist(rng);
        double qtyTemp = std::max(0.0001, qtyDist(rng));
        int randomSide = sideDist(rng);

        Order::OrderType side = (randomSide == 1) ? Order::OrderType::sell : Order::OrderType::buy;
        orders.push_back(Order(side, priceTemp, qtyTemp, 1, 1));
    }
}
