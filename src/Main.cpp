#include "Order.h"
#include "Utils.h"
#include <iostream>
#include "OrderBook.h"
#include <queue>
#include <ctime>
#include <chrono>
#include <thread>
#include <vector>
#include <sqlite3.h>
#include "MatchingEngine.h"
#include "PriceGenerator.h"

int main() {
    PriceGenerator gen {PriceGenerator()};

    gen.generateOrders(100);

    std::vector<Order> arr = gen.getOrders();

    std::cout << arr.size() << std::endl;

}