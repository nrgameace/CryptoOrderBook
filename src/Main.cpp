#include "Order.h"
#include <iostream>
#include "OrderBook.h"
#include <queue>
#include <ctime>
#include <chrono>

int main() {
    Order::OrderType type = Order::OrderType::buy;
    Order order1 {Order(type, 20.0, 1.5, 101, 111)};
    Order order2 {Order(type, 20.0, 1.0, 102, 111)};
    Order order3 {Order(type, 30.0, 1.0, 103, 111)};
    Order order4 {Order(type, 30.0, 1.0, 104, 111)};

    Order::OrderType type2 = Order::OrderType::sell;
    Order order5 {Order(type2, 10.0, 2.0, 111, 111)};
    Order order6 {Order(type2, 20.0, 1.0, 112, 111)};
    Order order7 {Order(type2, 35.0, 1.0, 113, 111)};
    Order order8 {Order(type2, 35.0, 1.0, 114, 111)};

    


    OrderBook book {OrderBook()};

    book.addOrder(order1);
    book.addOrder(order2);
    book.addOrder(order3);
    book.addOrder(order4);
    book.addOrder(order5);
    book.addOrder(order6);
    book.addOrder(order7);
    book.addOrder(order8);


    book.processOrder(order1, order5);
    std::cout << "Buy order: " << order1.quantity << std::endl;
    std::cout << "Sell order: " << order5.quantity << std::endl;
    


    auto buyBook = book.getBuyOffers();
    auto sellBook = book.getSellOffers();
    //Check for proper ordering
    for (auto& pair : sellBook) {
        //std::cout << pair.first << std::endl;
        auto tempQueue = pair.second;
        while (!tempQueue.empty()) {
            auto order = tempQueue.top();
            auto time = tempQueue.top().timestamp;
            
            //std::cout << tempQueue.size() << " size of queue" << std::endl;
            //std::time_t t = std::chrono::system_clock::to_time_t(time);
            //std::cout << std::ctime(&t);
            //std::cout << order.transactionId << std::endl;
            tempQueue.pop();
        }
    }
    //std::cout << "Sell book" << std::endl;

    //for (auto& pair : sellBook) {
      //  std::cout << pair.first << std::endl;
        
    //}

    //std::cout << buyBook.size() << std::endl;
    //std::cout << sellBook.size() << std::endl;


}