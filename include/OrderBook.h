#pragma once
#include <queue>
#include <map>
#include "Order.h"
#include <string>
#include <sqlite3.h>
class OrderBook {
    //std::queue<Order> buyOffers;
    //std::priority_queue<Order> sellOffers;

    std::map<double, std::priority_queue<Order>, std::greater<double>> buyOffers;
    std::map<double, std::priority_queue<Order>> sellOffers;
    

    public:
        sqlite3* db;
        OrderBook();
        ~OrderBook();
        bool addOrder(const Order& order);
        const std::map<double, std::priority_queue<Order>, std::greater<double>>& getBuyOffers();
        const std::map<double, std::priority_queue<Order>>& getSellOffers();
        void checkSellOrders();
        bool simulateMarket();
        bool processOrder(Order& orderBuy, Order& orderSell);
        void logTrade(sqlite3* db, int buyUserId, int sellUserId, double buyQuantity, double sellQuantity, double price, int timestamp);
        

};