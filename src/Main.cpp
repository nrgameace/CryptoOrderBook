#include "OrderBook.h"
#include "MatchingEngine.h"
#include "CoinMarketCapFetcher.h"
#include "TransactionLogger.h"
#include "TransactionLoggerInterface.h"
#include "Simulator.h"
#include <thread>
#include <iostream>

int main() {
    CoinMarketCapFetcher fetcher;

    TransactionLogger log;
    TransactionLoggerInterface& logPerm = log;

    OrderBook book;
    MatchingEngine eng(book, logPerm);

    Simulator simulator(eng, fetcher);

    std::thread stopper([&] {
        std::cin.get();
        simulator.stop();
    });

    simulator.start();
    stopper.join();
}
