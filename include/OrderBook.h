#pragma once
#include <queue>
#include <map>
#include "Order.h"
#include <string>
class OrderBook {
    //std::queue<Order> buyOffers;
    //std::priority_queue<Order> sellOffers;

    std::map<double, std::priority_queue<Order>, std::greater<double>> buyOffers;
    std::map<double, std::priority_queue<Order>> sellOffers;

    public:
        OrderBook();
        bool addOrder(const Order& order);
        const std::map<double, std::priority_queue<Order>, std::greater<double>>& getBuyOffers();
        const std::map<double, std::priority_queue<Order>>& getSellOffers();
        void checkSellOrders();
        bool simulateMarket();

};