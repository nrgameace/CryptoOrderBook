#pragma once
#include "TransactionLoggerInterface.h"
#include "OrderBook.h"
#include "Order.h"

class MarketStats;

class MatchingEngine {
    public:
        struct TradeResult { int64_t price; int64_t quantity; };

    private:
        OrderBook& book;
        TransactionLoggerInterface& log;
        TradeResult executeTrade(Order& buy, Order& sell, int64_t tradePrice);

    public:
        MatchingEngine(OrderBook& book, TransactionLoggerInterface& log);
        TradeResult processOrder(Order& orderBuy, Order& orderSell);
        bool simulateMarket(MarketStats* stats = nullptr);
        const OrderBook& getBook();
        const TransactionLoggerInterface& getLog();
        void addOrder(Order& order);


};