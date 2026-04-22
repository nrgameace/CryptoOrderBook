#pragma once
#include "IPriceFetcher.h"
#include "Order.h"
#include <vector>
#include <random>

class PriceGenerator {
    IPriceFetcher& fetcher;
    double currentPrice;
    std::vector<Order> orders;
    std::mt19937 rng;

    void updatePrice();

public:
    PriceGenerator(IPriceFetcher& fetcher);
    std::vector<Order> getOrders();
    void generateOrders(int numOrders);
};
