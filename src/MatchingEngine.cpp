#include "MatchingEngine.h"
#include "OrderBook.h"
#include "Order.h"
#include "TransactionLogger.h"
#include <stdexcept>

MatchingEngine::MatchingEngine(OrderBook& book)
    : book(book), log(TransactionLogger()) 
    {}


bool MatchingEngine::executeTrade(Order& buy, Order& sell, int64_t tradePrice) {
    int64_t difference = sell.quantity - buy.quantity;
    int64_t tradeQty = (difference <= 0) ? sell.quantity : buy.quantity;

    log.logTrade(buy.userId, sell.userId, tradeQty, tradePrice, std::time(nullptr));

    if (difference == 0) {
        buy.quantity = 0;
        sell.quantity = 0;
        return true;
    }

    if (difference < 0) {
        buy.quantity = buy.quantity - sell.quantity;
        sell.quantity = 0;
    } else {
        sell.quantity = sell.quantity - buy.quantity;
        buy.quantity = 0;
    }
    return false;
}

bool MatchingEngine::processOrder(Order& orderBuy, Order& orderSell) {
    if (orderBuy.transactionSide == Order::OrderType::sell || orderSell.transactionSide == Order::OrderType::buy) {
        throw std::runtime_error("Wrong side is in one order.");
    }

    if (orderBuy.price < orderSell.price) {
        return false;
    }

    int64_t tradePrice = (orderSell.getTimestamp() < orderBuy.getTimestamp()) ? orderSell.price : orderBuy.price;
    return executeTrade(orderBuy, orderSell, tradePrice);
}




/**
 * @brief simulates the entire market by repeadetly processing orders based on timestamp and price
 * @return True if all orders have been fufilled and market is empty, False if not
 */
bool MatchingEngine::simulateMarket() {
    // Goes until one hashmap is empty
    // While loop that goes through and processes the order using the function
    // Return true if both sides are empty return false if not
    // After market open, on new order added run simulate Market
    // Go until the market is empty or buy price is less than sell price
    while (!book.buyOffers.empty() && !book.sellOffers.empty()) {

        // Create an iterator to go through each pair in market
        auto iteratorBuyOffers = book.buyOffers.begin();
        auto iteratorSellOffers = book.sellOffers.begin();

        // If queue is empty, erase the entire key-value pair
        if (iteratorBuyOffers->second.empty()) {
            book.buyOffers.erase(iteratorBuyOffers);
            continue;
        }
        if (iteratorSellOffers->second.empty()) {
            book.sellOffers.erase(iteratorSellOffers);
            continue;
        }

        // Get orders and check
        Order highestBuy = iteratorBuyOffers->second.top();
        Order lowestSell = iteratorSellOffers->second.top();

        if (highestBuy.price < lowestSell.price) {
            break;
        }

        
        processOrder(highestBuy, lowestSell);

        // Check to see if quantity is equal to zero, if it is, remove off the hashmap
        iteratorBuyOffers->second.pop();
        if (highestBuy.quantity > 0)
            iteratorBuyOffers->second.push(highestBuy);
        else if (iteratorBuyOffers->second.empty())
            book.buyOffers.erase(iteratorBuyOffers);

        iteratorSellOffers->second.pop();
        if (lowestSell.quantity > 0)
            iteratorSellOffers->second.push(lowestSell);
        else if (iteratorSellOffers->second.empty())
            book.sellOffers.erase(iteratorSellOffers);

    }

    if (book.buyOffers.empty() && book.sellOffers.empty()){
        //std::cout << "Market is fully empty" << std::endl;
        return true;
    }
    else {
        //std::cout << "Market is not fully empty" << std::endl;
        return false;
    }
    
}


const OrderBook& MatchingEngine::getBook() {
    return book;
}

const TransactionLogger& MatchingEngine::getLog() {
    return log;
}