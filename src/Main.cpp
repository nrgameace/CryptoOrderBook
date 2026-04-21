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
#include "MarketStats.h"
#include "PriceGenerator.h"
#include "TransactionLogger.h"
#include "TransactionLoggerInterface.h"
#include "httplib.h"
#include "Simulator.h"

int main() {

    PriceGenerator gen {PriceGenerator()};

    gen.generateOrders(100);

    std::vector<Order> arr = gen.getOrders();

    TransactionLogger log {TransactionLogger()};
    
    TransactionLoggerInterface& logPerm = log;


    OrderBook book {OrderBook()};
    MatchingEngine eng {MatchingEngine(book, logPerm)};

    Simulator simulator {Simulator(eng)};


    std::thread stopper([&] {
        std::cin.get();  // wait for Enter key
        simulator.stop();
    });

    simulator.start();
    stopper.join();

    /** 
    PriceGenerator gen {PriceGenerator()};

    gen.generateOrders(100);

    std::vector<Order> arr = gen.getOrders();

    TransactionLogger log {TransactionLogger()};
    
    TransactionLoggerInterface& logPerm = log;


    OrderBook book {OrderBook()};
    MatchingEngine eng {MatchingEngine(book, logPerm)};

    for (Order order : arr) {
        book.addOrder(order);
    }

    MarketStats stats;
    eng.simulateMarket(&stats);

    std::cout << arr.size() << std::endl;
    
    return 0;
    */

}