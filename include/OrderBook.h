#pragma once
#include <queue>
#include "Order.h"
#include <string>
class OrderBook {
    std::queue<Order> buyOffers;
    std::priority_queue<Order> sellOffers;

    public:
        OrderBook(const std::queue<Order>& buyOffersTemp, const std::priority_queue<Order>& sellOffersTemp);
        bool addOrder(const std::string& side, const Order& order);
        std::queue<Order> getBuyOffers();
        std::priority_queue<Order> getSellOffers();

};