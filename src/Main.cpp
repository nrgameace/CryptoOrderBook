#include "Order.h"
#include <iostream>
#include "OrderBook.h"
#include <queue>

int main() {
    Order::OrderType type = Order::OrderType::buy;
    Order order1 {Order(type, 1.0, 1.0, 111, 111)};
    Order order2 {Order(type, 1.0, 1.0, 111, 111)};
    Order order3 {Order(type, 1.0, 1.0, 111, 111)};
    Order order4 {Order(type, 1.0, 1.0, 111, 111)};

    Order::OrderType type2 = Order::OrderType::sell;
    Order order5 {Order(type2, 1.0, 1.0, 111, 111)};
    Order order6 {Order(type2, 1.0, 1.0, 111, 111)};
    Order order7 {Order(type2, 1.0, 1.0, 111, 111)};
    Order order8 {Order(type2, 1.0, 1.0, 111, 111)};

    std::queue<Order> buy;
    std::priority_queue<Order> sell;

    buy.push(order1);
    buy.push(order2);
    buy.push(order3);
    buy.push(order4);

    sell.push(order5);
    sell.push(order6);
    sell.push(order7);
    sell.push(order8);

    OrderBook book {OrderBook(buy,sell)};
}