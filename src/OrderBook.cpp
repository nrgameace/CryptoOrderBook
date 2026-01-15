#include "OrderBook.h"


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