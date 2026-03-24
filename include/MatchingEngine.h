#pragma once
#include "TransactionLogger.h"
#include "OrderBook.h"
#include "Order.h"
class MatchingEngine {

    OrderBook& book;
    TransactionLogger log;
    bool executeTrade(Order& buy, Order& sell, int64_t tradePrice);

    public:
    MatchingEngine(OrderBook& book);
    bool processOrder(Order& orderBuy, Order& orderSell);
    bool simulateMarket();
    const OrderBook& getBook();
    const TransactionLogger& getLog();


};