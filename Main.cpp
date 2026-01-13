#include "OrderBook.h"
#include <iostream>

int main() {
    Order one = Order(5.0, "Buy", true, 100);
    std::cout << one.buyOffer;
}