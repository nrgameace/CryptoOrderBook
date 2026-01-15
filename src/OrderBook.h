#pragma once
#include <queue>
#include "Order.h"
#include <string>
class OrderBook {
    std::queue<Order> buyOffers;
    std::priority_queue<Order> sellOffers;

    public:
        bool addOrder(const std::string& side, const Order& order);

};