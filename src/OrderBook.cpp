#include "OrderBook.h"
#include <queue>
#include "Order.h"


OrderBook::OrderBook(const std::queue<Order>& buyOffersTemp, const std::priority_queue<Order>& sellOffersTemp) 
    : buyOffers(buyOffersTemp), sellOffers(sellOffersTemp)
{}

bool OrderBook::addOrder(const std::string& side, const Order& order) {
    if (side == "buy") {
        buyOffers.push(order);
        return true;
    }
    else if (side == "sell") {
        sellOffers.push(order);
        return true;
    }
    else 
        return false;
}