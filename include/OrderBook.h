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
        std::map<int64_t, std::priority_queue<Order>, std::greater<int64_t>> buyOffers;
        std::map<int64_t, std::priority_queue<Order>> sellOffers;

    

        OrderBook();
        bool addOrder(const Order& order);
        const std::map<int64_t, std::priority_queue<Order>, std::greater<int64_t>>& getBuyOffers();
        const std::map<int64_t, std::priority_queue<Order>>& getSellOffers();
        bool operator==(const OrderBook& other) const;

        
        
};