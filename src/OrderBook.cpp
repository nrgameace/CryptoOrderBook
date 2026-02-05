#include "OrderBook.h"
#include "Order.h"
#include <queue>
#include <iostream>
#include <map>


OrderBook::OrderBook() 
    : buyOffers(), sellOffers()
{}

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
    auto iteratorBuyOffers = buyOffers.begin();
    auto iteratorSellOffers = sellOffers.begin();
    while (!buyOffers.empty() && !sellOffers.empty())
    {

        if (iteratorBuyOffers->second.empty()) {
            ++iteratorBuyOffers;
        }
        Order highestBuy = (iteratorBuyOffers->second).top();

        if (iteratorSellOffers->second.empty()) {
            ++iteratorSellOffers;
        }
        Order lowestSell = (iteratorSellOffers->second).top();


        std::cout << "Buy offer quantity: " << highestBuy.quantity << std::endl;
        std::cout << "Sell offer quantity: " << lowestSell.quantity << std::endl;


        processOrder(highestBuy, lowestSell);

        if (std::abs(highestBuy.quantity) <= EPS) {
            std::cout << "Pop buy offer" << std::endl;
            iteratorBuyOffers->second.pop();
        }
        if (std::abs(lowestSell.quantity) <= EPS) {
            iteratorSellOffers->second.pop();
            std::cout << "Pop sell offer" << std::endl;
        }
        
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



