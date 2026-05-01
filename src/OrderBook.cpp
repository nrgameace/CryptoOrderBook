#include "OrderBook.h"
#include "Order.h"
#include <queue>
#include <map>
#include <ctime>
#include <shared_mutex>
#include <mutex>

OrderBook::OrderBook() 
    : buyOffers(), sellOffers()
{};


/**
 * @brief Adds an order to the respective side and priority queue based on price
 * @param order Pass through a singular order by constant reference in order to add it to the market
 * @return True if the order was successfully added, False if not
 */
void OrderBook::addOrder(const Order& order) {
    std::unique_lock<std::shared_mutex> lock(mtx);

    if (order.transactionSide == Order::OrderType::buy) {
        buyOffers[order.price].push(order);
    } else {
        sellOffers[order.price].push(order);
    }
}

Order OrderBook::getBestBid() const {
    std::shared_lock<std::shared_mutex> lock(mtx);

    if (buyOffers.empty() || buyOffers.begin()->second.empty()) {
        throw std::runtime_error("Buy side is empty");
    }
    return buyOffers.begin()->second.top();
}

Order OrderBook::getBestAsk() const {
    std::shared_lock<std::shared_mutex> lock(mtx);

    if (sellOffers.empty() || sellOffers.begin()->second.empty()) {
        throw std::runtime_error("Sell side is empty");
    }
    return sellOffers.begin()->second.top();
}

void OrderBook::removeBestBid() {
    std::unique_lock<std::shared_mutex> lock(mtx);
    if (buyOffers.empty() || buyOffers.begin()->second.empty())
        throw std::runtime_error("Buy side is empty");
    auto it = buyOffers.begin();
    it->second.pop();
    if (it->second.empty()) buyOffers.erase(it);
}

void OrderBook::removeBestAsk() {
    std::unique_lock<std::shared_mutex> lock(mtx);
    if (sellOffers.empty() || sellOffers.begin()->second.empty())
        throw std::runtime_error("Sell side is empty");
    auto it = sellOffers.begin();
    it->second.pop();
    if (it->second.empty()) sellOffers.erase(it);
}

bool OrderBook::isBuySideEmpty() const {
    std::shared_lock<std::shared_mutex> lock(mtx);
    return buyOffers.empty();
}

bool OrderBook::isSellSideEmpty() const {
    std::shared_lock<std::shared_mutex> lock(mtx);
    return sellOffers.empty();
}

int OrderBook::getBuyDepth() const {
    std::shared_lock<std::shared_mutex> lock(mtx);
    return static_cast<int>(buyOffers.size());
}

int OrderBook::getSellDepth() const {
    std::shared_lock<std::shared_mutex> lock(mtx);
    return static_cast<int>(sellOffers.size());
}

