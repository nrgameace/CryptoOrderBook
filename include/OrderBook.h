#pragma once
#include <queue>
#include <map>
#include "Order.h"
class OrderBook {


    std::map<int64_t, std::priority_queue<Order>, std::greater<int64_t>> buyOffers;
    std::map<int64_t, std::priority_queue<Order>> sellOffers;

    
    public:
        OrderBook();
        bool addOrder(const Order& order);
        bool operator==(const OrderBook& other) const;
        Order getBestBid();
        Order getBestAsk();
        void removeBestBid();
        void removeBestAsk();
        bool isBuySideEmpty();
        bool isSellSideEmpty();
        int getBuyDepth();
        int getSellDepth();

        
        
};