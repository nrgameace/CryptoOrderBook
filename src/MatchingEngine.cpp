#include "MatchingEngine.h"
#include "MarketStats.h"
#include "OrderBook.h"
#include "Order.h"
#include "TransactionLogger.h"
#include <stdexcept>

MatchingEngine::MatchingEngine(OrderBook& book, TransactionLoggerInterface& log)
    : book(book), log(log) 
    {}


MatchingEngine::TradeResult MatchingEngine::executeTrade(Order& buy, Order& sell, int64_t tradePrice) {
    int64_t difference = sell.quantity - buy.quantity;
    int64_t tradeQty = (difference <= 0) ? sell.quantity : buy.quantity;

    log.logTrade(buy.userId, sell.userId, tradeQty, tradePrice, static_cast<int64_t>(std::time(nullptr)));

    if (difference == 0) {
        buy.quantity = 0;
        sell.quantity = 0;
        return TradeResult{tradePrice, tradeQty};
    }

    if (difference < 0) {
        buy.quantity = buy.quantity - sell.quantity;
        sell.quantity = 0;
    } else {
        sell.quantity = sell.quantity - buy.quantity;
        buy.quantity = 0;
    }
    return TradeResult{tradePrice, tradeQty};
}

MatchingEngine::TradeResult MatchingEngine::processOrder(Order& orderBuy, Order& orderSell) {
    if (orderBuy.transactionSide == Order::OrderType::sell || orderSell.transactionSide == Order::OrderType::buy) {
        throw std::runtime_error("Wrong side is in one order.");
    }

    if (orderBuy.price < orderSell.price) {
        return TradeResult{0, 0};
    }

    int64_t tradePrice = (orderSell.getTimestamp() < orderBuy.getTimestamp()) ? orderSell.price : orderBuy.price;
    return executeTrade(orderBuy, orderSell, tradePrice);
}




/**
 * @brief simulates the entire market by repeadetly processing orders based on timestamp and price
 * @return True if all orders have been fufilled and market is empty, False if not
 */
bool MatchingEngine::simulateMarket(MarketStats* stats) {
    while (!book.isBuySideEmpty() && !book.isSellSideEmpty()) {
        Order highestBuy = book.getBestBid();
        Order lowestSell = book.getBestAsk();

        if (highestBuy.price < lowestSell.price) break;

        auto result = processOrder(highestBuy, lowestSell);
        if (result.quantity == 0) break;
        if (stats) stats->recordTrade(result.price, result.quantity);

        book.removeBestBid();
        if (highestBuy.quantity > 0) book.addOrder(highestBuy);

        book.removeBestAsk();
        if (lowestSell.quantity > 0) book.addOrder(lowestSell);
    }

    if (stats) {
        if (!book.isBuySideEmpty() && !book.isSellSideEmpty())
            stats->updateSpread(book.getBestBid().price, book.getBestAsk().price);
        stats->printSummary();
    }

    return book.isBuySideEmpty() && book.isSellSideEmpty();
}


const OrderBook& MatchingEngine::getBook() {
    return book;
}

const TransactionLoggerInterface& MatchingEngine::getLog() {
    return log;
}

void MatchingEngine::addOrder(Order& order) {
    book.addOrder(order);
}