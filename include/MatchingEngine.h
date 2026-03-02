#pragma once
#include "OrderBook.h"
#include "TransactionLogger.h"

class MatchingEngine {

    TransactionLogger log;

    MatchingEngine(OrderBook& book, TransactionLogger& log);
    bool processOrder(Order& orderBuy, Order& orderSell);


};