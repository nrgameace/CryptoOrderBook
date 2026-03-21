#include "MatchingEngine.h"
#include "OrderBook.h"
#include "Order.h"
#include "TransactionLogger.h"
#include <iostream>
#include <stdexcept>

MatchingEngine::MatchingEngine(OrderBook& book)
    : book(book), log(TransactionLogger()) 
    {}


/**
 * @brief Processes two orders one from each side at a time
 * @param orderBuy buy order that is being processed
 * @param orderSell sell order that is being processed
 * @return True if both orders are fully fufilled, False if not
 */
bool MatchingEngine::processOrder(Order& orderBuy, Order& orderSell) {
    //Go to top of hashmap for both
    //Find which one was the resting price (created earlier)
    //Excecute trade -- if not enough quantity, go to next
    if (orderBuy.transactionSide == Order::OrderType::sell || orderSell.transactionSide == Order::OrderType::buy) {
        throw std::runtime_error("Wrong side is in one order.");
    } 

    
    //Ensure that buy offer is at least greater than or equal to the sell price
    if (orderBuy.price >= orderSell.price) {

    // Check which order was a resting order and which was a incoming order
        if (orderSell.getTimestamp() < orderBuy.getTimestamp()) {

            double difference = orderSell.quantity - orderBuy.quantity;

            constexpr double EPS = 1e-2;  
            auto time = std::time(nullptr);
            if (difference <= 0)
                log.logTrade(orderBuy.userId, orderSell.userId, orderSell.quantity, orderSell.price, time);
            else
                log.logTrade(orderBuy.userId, orderSell.userId, orderBuy.quantity, orderSell.price, time);

            if (std::abs(difference) < EPS) {
                orderBuy.quantity = 0.0;
                orderSell.quantity = 0.0;
                //std::cout << "The offers are perfectly mathced" << std::endl;
                return true;
            }

            else if (difference < 0) {
                double quantityLeftOver = orderBuy.quantity - orderSell.quantity;
                orderBuy.quantity = quantityLeftOver;
                orderSell.quantity = 0.0;
                //std::cout << "The buy offer was greater and " << orderBuy.quantity << " is left over. Transaction ID: " << orderBuy.transactionId << std::endl;
                return false;
            }
            else {
                double quantityLeftOver = orderSell.quantity - orderBuy.quantity;
                orderSell.quantity = quantityLeftOver;
                orderBuy.quantity = 0.0;
                //std::cout << "The sell offer was greater and " << orderSell.quantity << " is left over. Transaction ID: "<< orderSell.transactionId << std::endl;
                return false;
            }

        }
        else {
            double difference = orderSell.quantity - orderBuy.quantity;


            auto time = std::time(nullptr);
            if (difference <= 0)
                log.logTrade(orderBuy.userId, orderSell.userId, orderBuy.quantity, orderBuy.price, time);
            else
                log.logTrade(orderBuy.userId, orderSell.userId, orderSell.quantity, orderBuy.price, time);

            constexpr double EPS = 1e-2;   
            if (std::abs(difference) < EPS) {
                orderBuy.quantity = 0.0;
                orderSell.quantity = 0.0;
                //std::cout << "The offers are perfectly matched" << std::endl;
                return true;
            }

            else if (difference < 0) {
                double quantityLeftOver = orderBuy.quantity - orderSell.quantity;
                orderBuy.quantity = quantityLeftOver;
                orderSell.quantity = 0.0;
                //std::cout << "The buy offer was greater and " << orderBuy.quantity << " is left over. Transaction ID: " << orderBuy.transactionId << std::endl;
                return false;
            }
            else {
                double quantityLeftOver = orderSell.quantity - orderBuy.quantity;
                orderSell.quantity = quantityLeftOver;
                orderBuy.quantity = 0.0;
                //std::cout << "The sell offer was greater and " << orderSell.quantity << " is left over. Transaction ID: "<< orderSell.transactionId << std::endl;
                return false;
            }
        }
    }
    
    return false;


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
    constexpr double EPS = 1e-9;


    // Go until the market is empty or biy price is less than sell price
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
        if (highestBuy.quantity > EPS)
            iteratorBuyOffers->second.push(highestBuy);
        else if (iteratorBuyOffers->second.empty())
            book.buyOffers.erase(iteratorBuyOffers);

        iteratorSellOffers->second.pop();
        if (lowestSell.quantity > EPS)
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