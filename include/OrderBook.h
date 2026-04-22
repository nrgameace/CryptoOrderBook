#pragma once
#include <queue>
#include <map>
#include <shared_mutex>
#include "Order.h"
class OrderBook {

    struct OrderTimePriority {
        bool operator()(const Order& a, const Order& b) const { return a.timestamp > b.timestamp; }
    };

    std::map<int64_t, std::priority_queue<Order, std::vector<Order>, OrderTimePriority>, std::greater<int64_t>> buyOffers;
    std::map<int64_t, std::priority_queue<Order, std::vector<Order>, OrderTimePriority>> sellOffers;
    mutable std::shared_mutex mtx;


    public:
        OrderBook();
        void addOrder(const Order& order);
        Order getBestBid() const;
        Order getBestAsk() const;
        void removeBestBid();
        void removeBestAsk();
        bool isBuySideEmpty() const;
        bool isSellSideEmpty() const;
        int getBuyDepth() const;
        int getSellDepth() const;

        
        
};