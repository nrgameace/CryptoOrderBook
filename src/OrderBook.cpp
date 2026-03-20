#include "OrderBook.h"
#include "Order.h"
#include "MatchingEngine.h"
#include <queue>
#include <iostream>
#include <map>
#include <sqlite3.h>
#include <ctime>


OrderBook::OrderBook() 
    : buyOffers(), sellOffers()
{};


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

bool OrderBook::operator==(const OrderBook& other) {
    if (sellOffers.size() != other.sellOffers.size()) {
        return false;
    }

    if (buyOffers.size() != other.buyOffers.size()) {
        return false;
    }


    for (auto& [price, queue] : sellOffers) {
        if (other.sellOffers.find(price) == other.sellOffers.end())
            return false;
        
        auto otherQueue = other.sellOffers.at(price);

        if (queue.size() != otherQueue.size()) return false;

        while (!queue.empty()) {
            if (!(queue.top() == otherQueue.top()))
                return false;
            queue.pop();
            otherQueue.pop();
        }


    }

    return true;
}
