#include "OrderBook.h"
#include <iostream>

int main() {
    Order one = Order(5.0, "Buy", true, 100);
    Order two = Order(7.0,"Sell",0,123);
    std::cout << one.isBuyOffer << std::endl;
    std::cout << two.isBuyOffer << std::endl;
}