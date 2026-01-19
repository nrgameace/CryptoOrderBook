#include "OrderBook.h"
#include "Order.h"
#include <queue>
#include <iostream>


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

std::queue<Order> OrderBook::getBuyOffers() {
    return buyOffers;
}

std::priority_queue<Order> OrderBook::getSellOffers() {
    return sellOffers;
}

void OrderBook::checkSellOrders() {
    std::priority_queue<Order> sellOffersTemp {sellOffers};
    int length {static_cast<int>(sellOffersTemp.size())};
    for (int i{}; i < length; i++) {
        std::cout << (sellOffersTemp.top().price) << std::endl;
        sellOffersTemp.pop();
    }
}

