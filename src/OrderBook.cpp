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
    std::vector<int64_t> keys;

    if (order.transactionSide == Order::OrderType::buy) {
        for (const auto& pair: buyOffers) {
            keys.push_back(pair.first);
        }

        for (int64_t key : keys) {
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
        
        for (int64_t key : keys) {
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
const std::map<int64_t, std::priority_queue<Order>, std::greater<int64_t>>& OrderBook::getBuyOffers() {
    return buyOffers;
}

/**
 * @brief Accessor method for all sell offers
 * @return A copy of the all the sell offers in the orderbook
 */
const std::map<int64_t, std::priority_queue<Order>>& OrderBook::getSellOffers() {
    return sellOffers;
}

bool OrderBook::operator==(const OrderBook& other) const{
    if (sellOffers.size() != other.sellOffers.size()) {
        return false;
    }

    if (buyOffers.size() != other.buyOffers.size()) {
        return false;
    }


    for (auto& [price, queue] : sellOffers) {
        if (other.sellOffers.find(price) == other.sellOffers.end())
            return false;
        
        auto q1 = queue;
        auto q2 = other.sellOffers.at(price);

        if (q1.size() != q2.size()) return false;

        while (!q1.empty()) {
            if (!(q1.top() == q2.top()))
                return false;
            q1.pop();
            q2.pop();
        }
    }

    for (auto& [price, queue] : buyOffers) {
        if (other.buyOffers.find(price) == other.buyOffers.end())
            return false;
        
        auto q1 = queue;
        auto q2 = other.buyOffers.at(price);

        if (q1.size() != q2.size()) return false;

        while (!q1.empty()) {
            if (!(q1.top() == q2.top()))
                return false;
            q1.pop();
            q2.pop();
        }
    }

    return true;
}
