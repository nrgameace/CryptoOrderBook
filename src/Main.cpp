#include "Order.h"
#include "Utils.h"
#include <iostream>
#include "OrderBook.h"
#include <queue>
#include <ctime>
#include <chrono>
#include <thread>
#include <sqlite3.h>
#include "MatchingEngine.h"
#include "PriceGenerator.h"

int main() {
    PriceGenerator gen {PriceGenerator()};

    gen.updatePrice();
    
    std::cout << "No error" << std::endl;
    return 0;

}