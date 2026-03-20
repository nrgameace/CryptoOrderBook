#pragma once
#include <queue>
#include <map>
#include "Order.h"
#include <string>
#include <sqlite3.h>
class OrderBook {
    //std::queue<Order> buyOffers;
    //std::priority_queue<Order> sellOffers;
    public:
        std::map<double, std::priority_queue<Order>, std::greater<double>> buyOffers;
        std::map<double, std::priority_queue<Order>> sellOffers;

    

        OrderBook();
        bool addOrder(const Order& order);
        const std::map<double, std::priority_queue<Order>, std::greater<double>>& getBuyOffers();
        const std::map<double, std::priority_queue<Order>>& getSellOffers();
        bool operator==(const OrderBook& other);

        
        
};