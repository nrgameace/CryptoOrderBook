#pragma once
#include "TransactionLoggerInterface.h"
#include "OrderBook.h"
#include "Order.h"
class MatchingEngine {

    OrderBook& book;
    TransactionLoggerInterface& log;
    bool executeTrade(Order& buy, Order& sell, int64_t tradePrice);

    public:
        MatchingEngine(OrderBook& book, TransactionLoggerInterface& log);
        bool processOrder(Order& orderBuy, Order& orderSell);
        bool simulateMarket();
        const OrderBook& getBook();
        const TransactionLoggerInterface& getLog();


};