#pragma once
#include "IPriceFetcher.h"
#include "Order.h"
#include <vector>

class PriceGenerator {
    IPriceFetcher& fetcher;
    double currentPrice;
    std::vector<Order> orders;

    void updatePrice();

public:
    PriceGenerator(IPriceFetcher& fetcher);
    std::vector<Order> getOrders();
    void generateOrders(int numOrders);
};
