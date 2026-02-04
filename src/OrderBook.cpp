#include "OrderBook.h"
#include "Order.h"
#include <queue>
#include <iostream>
#include <map>


OrderBook::OrderBook() 
    : buyOffers(), sellOffers()
{}

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

const std::map<double, std::priority_queue<Order>, std::greater<double>>& OrderBook::getBuyOffers() {
    return buyOffers;
}

const std::map<double, std::priority_queue<Order>>& OrderBook::getSellOffers() {
    return sellOffers;
}

bool OrderBook::processOrder(Order& orderBuy, Order& orderSell) {
    //Go to top of hashmap for both
    //Find which one was the resting price (created earlier)
    //Excecute trade -- if not enough quantity, go to next
    
    //Ensure that buy offer is at least greater than or equal to the sell price
    if (orderBuy.price >= orderSell.price) {

    // Check which order was a resting order and which was a incoming order
        if (orderSell.getTimestamp() < orderBuy.getTimestamp()) {
            // Check to see 
            double sellValue = orderSell.price * orderSell.quantity;
            double buyValue = orderSell.price * orderBuy.quantity;
            double difference = sellValue - buyValue;

            constexpr double EPS = 1e-9;  
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
                std::cout << "The buy offer was greater and " << orderBuy.quantity << " is left over."<<std::endl;
                return false;
            }
            else {
                double quantityLeftOver = orderSell.quantity - orderBuy.quantity;
                orderSell.quantity = quantityLeftOver;
                orderBuy.quantity = 0.0;
                std::cout << "The sell offer was greater and " << orderSell.quantity << " is left over."<<std::endl;
                return false;
            }

        }
        else {
            double sellValue = orderBuy.price * orderSell.quantity;
            double buyValue = orderBuy.price * orderBuy.quantity;
            double difference = sellValue - buyValue;

            constexpr double EPS = 1e-9;   
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
                std::cout << "The buy offer was greater and " << orderBuy.quantity << " is left over."<<std::endl;
                return false;
            }
            else {
                double quantityLeftOver = orderSell.quantity - orderBuy.quantity;
                orderSell.quantity = quantityLeftOver;
                orderBuy.quantity = 0.0;
                std::cout << "The sell offer was greater and " << orderSell.quantity << " is left over."<<std::endl;
                return false;
            }
        }
    }
    return false;

}

bool OrderBook::simulateMarket() {
    // Goes until one hashmap is empty
    // While loop that goes through and processes the order using the function
    // Return true if both sides are empty return false if not
    // After market open, on new order added run simulate Market
    while (!buyOffers.empty() && !sellOffers.empty())
    {
        auto iteratorBuyOffers = buyOffers.begin();
        auto iteratorSellOffers = sellOffers.begin();

        Order highestBuy = (iteratorBuyOffers->second).top();
        iteratorBuyOffers->second.pop();

        Order lowestSell = (iteratorSellOffers->second).top();
        iteratorSellOffers->second.pop();

        processOrder(highestBuy, lowestSell);
        if (highestBuy.quantity == 0.0)
            iteratorBuyOffers->second.pop();
        if (lowestSell.quantity == 0.0)
            iteratorSellOffers->second.pop();
        
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



