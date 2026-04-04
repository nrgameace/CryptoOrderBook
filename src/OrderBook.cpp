#include "OrderBook.h"
#include "Order.h"
#include "MatchingEngine.h"
#include <queue>
#include <iostream>
#include <map>
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

    if (order.transactionSide == Order::OrderType::buy) {

        buyOffers[order.price].push(order);
        return true;
    }

    else if (order.transactionSide == Order::OrderType::sell) {
        sellOffers[order.price].push(order);
        return true;
        
    }
    return false;
}

// For testing only 
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

Order OrderBook::getBestBid() {

    if (buyOffers.empty() || buyOffers.begin()->second.empty()) {
        throw std::runtime_error("Buy side is empty");
    }
    return buyOffers.begin()->second.top();
}

Order OrderBook::getBestAsk() {
    if (sellOffers.empty() || sellOffers.begin()->second.empty()) {
        throw std::runtime_error("Sell side is empty");
    }
    return sellOffers.begin()->second.top();
}

void OrderBook::removeBestBid() {
    if (buyOffers.empty() || buyOffers.begin()->second.empty())
        throw std::runtime_error("Buy side is empty");
    auto it = buyOffers.begin();
    it->second.pop();
    if (it->second.empty()) buyOffers.erase(it);
}

void OrderBook::removeBestAsk() {
    if (sellOffers.empty() || sellOffers.begin()->second.empty())
        throw std::runtime_error("Sell side is empty");
    auto it = sellOffers.begin();
    it->second.pop();
    if (it->second.empty()) sellOffers.erase(it);
}

bool OrderBook::isBuySideEmpty() { return buyOffers.empty(); }
bool OrderBook::isSellSideEmpty() { return sellOffers.empty(); }
int OrderBook::getBuyDepth() { return static_cast<int>(buyOffers.size()); }
int OrderBook::getSellDepth() { return static_cast<int>(sellOffers.size()); }

