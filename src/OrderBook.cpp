#include "OrderBook.h"
#include "Order.h"
#include <queue>
#include <iostream>
#include <map>
#include <sqlite3.h>
#include <ctime>


OrderBook::OrderBook() 
    : buyOffers(), sellOffers(), db(nullptr)
{
    int rc = sqlite3_open("Database/TransactionHistory.db", &db);
    if (rc != 0) {
        std::cout << "Error connecting to database: " << sqlite3_errmsg(db) << std::endl;    
        db = nullptr;
    }
    else {
        std::cout << "Database connection successful!" << std::endl;
    }
}

OrderBook::~OrderBook() {
    if (db)
        sqlite3_close(db);
}

/**
 * @brief Adds an order to the respective side and priority queue based on price
 * @param order Pass through a singular order by constant reference in order to add it to the market
 * @return True if the order was successfully added, False if not
 */
bool OrderBook::addOrder(const Order& order) {
    std::vector<double> keys;

    if (order.transactionSide == Order::OrderType::buy) {
        for (const auto& pair: buyOffers) {
            keys.push_back(pair.first);
        }
        
        for (double key : keys) {
            if (key == order.price) {
                buyOffers[key].push(order);
                return true;
            }
        }

        std::priority_queue<Order> tempOrderStore;
        tempOrderStore.push(order);
        buyOffers[order.price] = std::move(tempOrderStore);

        return true;
    }

    else if (order.transactionSide == Order::OrderType::sell) {
        for (const auto& pair: sellOffers) {
            keys.push_back(pair.first);
        }
        
        for (double key : keys) {
            if (key == order.price) {
                sellOffers[key].push(order);
                return true;
            }
        }
        std::priority_queue<Order> tempOrderStore;
        tempOrderStore.push(order);
        sellOffers[order.price] = std::move(tempOrderStore);

        return true;
        
    }
    return false;
}

/**
 * @brief Accessor method for all buy offers
 * @return A copy of the all the buy offers in the orderbook
 */
const std::map<double, std::priority_queue<Order>, std::greater<double>>& OrderBook::getBuyOffers() {
    return buyOffers;
}

/**
 * @brief Accessor method for all sell offers
 * @return A copy of the all the sell offers in the orderbook
 */
const std::map<double, std::priority_queue<Order>>& OrderBook::getSellOffers() {
    return sellOffers;
}

/**
 * @brief Processes two orders one from each side at a time
 * @param orderBuy buy order that is being processed
 * @param orderSell sell order that is being processed
 * @return True if both orders are fully fufilled, False if not
 */
bool OrderBook::processOrder(Order& orderBuy, Order& orderSell) {
    //Go to top of hashmap for both
    //Find which one was the resting price (created earlier)
    //Excecute trade -- if not enough quantity, go to next

    
    //Ensure that buy offer is at least greater than or equal to the sell price
    if (orderBuy.price >= orderSell.price) {

    // Check which order was a resting order and which was a incoming order
        if (orderSell.getTimestamp() < orderBuy.getTimestamp()) {

            double difference = orderSell.quantity - orderBuy.quantity;

            constexpr double EPS = 1e-9;  
            auto time = std::time(nullptr);
            if (difference <= 0)
                logTrade(db, orderBuy.userId, orderSell.userId, orderSell.quantity, orderSell.price, time);
            else
                logTrade(db, orderBuy.userId, orderSell.userId, orderBuy.quantity, orderSell.price, time);

            if (std::abs(difference) < EPS) {
                orderBuy.quantity = 0.0;
                orderSell.quantity = 0.0;
                std::cout << "The offers are perfectly mathced" << std::endl;
                return true;
            }

            else if (difference < 0) {
                double quantityLeftOver = orderBuy.quantity - orderSell.quantity;
                orderBuy.quantity = quantityLeftOver;
                orderSell.quantity = 0.0;
                std::cout << "The buy offer was greater and " << orderBuy.quantity << " is left over. Transaction ID: " << orderBuy.transactionId << std::endl;
                return false;
            }
            else {
                double quantityLeftOver = orderSell.quantity - orderBuy.quantity;
                orderSell.quantity = quantityLeftOver;
                orderBuy.quantity = 0.0;
                std::cout << "The sell offer was greater and " << orderSell.quantity << " is left over. Transaction ID: "<< orderSell.transactionId << std::endl;
                return false;
            }

        }
        else {
            double difference = orderSell.quantity - orderBuy.quantity;


            auto time = std::time(nullptr);
            if (difference <= 0)
                logTrade(db, orderBuy.userId, orderSell.userId, orderBuy.quantity, orderBuy.price, time);
            else
                logTrade(db, orderBuy.userId, orderSell.userId, orderSell.quantity, orderBuy.price, time);

            constexpr double EPS = 1e-2;   
            if (std::abs(difference) < EPS) {
                orderBuy.quantity = 0.0;
                orderSell.quantity = 0.0;
                std::cout << "The offers are perfectly matched" << std::endl;
                return true;
            }

            else if (difference < 0) {
                double quantityLeftOver = orderBuy.quantity - orderSell.quantity;
                orderBuy.quantity = quantityLeftOver;
                orderSell.quantity = 0.0;
                std::cout << "The buy offer was greater and " << orderBuy.quantity << " is left over. Transaction ID: " << orderBuy.transactionId << std::endl;
                return false;
            }
            else {
                double quantityLeftOver = orderSell.quantity - orderBuy.quantity;
                orderSell.quantity = quantityLeftOver;
                orderBuy.quantity = 0.0;
                std::cout << "The sell offer was greater and " << orderSell.quantity << " is left over. Transaction ID: "<< orderSell.transactionId << std::endl;
                return false;
            }
        }
    }
    
    return false;

}

/**
 * @brief Method that logs a transaction in the TRADES database
 * @param db Database pointer that maintains connection
 * @param buyUserId The buying user's ID pertaining to that transaction
 * @param sellUserId The selling user's ID that pertains to that transaction
 * @param buyQuantity The amount that was agreed upon in the trade
 * @param sellQuantity The amount the seller is trading
 * @param price The agreed upon price
 * @param timestamp The time at which the transaction occurred
 */
void OrderBook::logTrade(sqlite3* db, int buyUserId, int sellUserId, double quantity, double price, int timestamp) {
    const char* sql = "INSERT INTO TRADES (buyUserId, sellUserId, quantity, price, timestamp) VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;

    // 2. Prepare the SQL statement into a prepared statement object (sqlite3_stmt)
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    // 3. Bind C++ variables to the placeholders
    // The first parameter '1' refers to the first '?'
    sqlite3_bind_int(stmt, 1, buyUserId); 
    sqlite3_bind_int(stmt, 2, sellUserId);
    sqlite3_bind_double(stmt, 3, quantity);
    sqlite3_bind_double(stmt, 4, price);
    sqlite3_bind_int(stmt, 5, timestamp);

    // 4. Execute the statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Execution failed: " << sqlite3_errmsg(db) << std::endl;
    } else {
        std::cout << "Row inserted successfully." << std::endl;
    }

    sqlite3_finalize(stmt);

    
}



/**
 * @brief simulates the entire market by repeadetly processing orders based on timestamp and price
 * @return True if all orders have been fufilled and market is empty, False if not
 */
bool OrderBook::simulateMarket() {
    // Goes until one hashmap is empty
    // While loop that goes through and processes the order using the function
    // Return true if both sides are empty return false if not
    // After market open, on new order added run simulate Market
    constexpr double EPS = .001;

    
    

    // Go until the market is empty or biy price is less than sell price
    while (!buyOffers.empty() && !sellOffers.empty()) {

        // Create an iterator to go through each pair in market
        auto iteratorBuyOffers = buyOffers.begin();
        auto iteratorSellOffers = sellOffers.begin();

        // If queue is empty, erase the entire key-value pair
        if (iteratorBuyOffers->second.empty()) {
            buyOffers.erase(iteratorBuyOffers);
            continue;
        }
        if (iteratorSellOffers->second.empty()) {
            sellOffers.erase(iteratorSellOffers);
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
            buyOffers.erase(iteratorBuyOffers);

        iteratorSellOffers->second.pop();
        if (lowestSell.quantity > EPS)
            iteratorSellOffers->second.push(lowestSell);
        else if (iteratorSellOffers->second.empty())
            sellOffers.erase(iteratorSellOffers);

    }

    if (buyOffers.empty() && sellOffers.empty()){
        std::cout << "Market is fully empty" << std::endl;
        return true;
    }
    else {
        std::cout << "Market is not fully empty" << std::endl;
        return false;
    }
    
}



