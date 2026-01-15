#include "Order.h"
#include <iostream>

int main() {
    Order::OrderType type = Order::OrderType::buy;
    Order order1 {Order(type, 1.0, 1.0, 111, 111)};
    std::cout << order1.price << std::endl;
}