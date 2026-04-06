#include <iostream>
#include <chrono>
#include <random>
#include "Order.h"
#include "OrderBook.h"
#include "MockTransactionLogger.h"
#include "TransactionLoggerInterface.h"
#include "MatchingEngine.h"


void benchmarkAddOrder(int N) {
    OrderBook book;
   
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(90,110);

    auto start = std::chrono::high_resolution_clock::now();
    for (int i {}; i < N; i++) {
        book.addOrder(Order(Order::OrderType::buy, dist(rng), 1.0, i, i));
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto total = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

    std::cout << "addOrder x" << N
              << " | total: " << total.count() << " ns"
              << " | mean: "  << total.count() / N << " ns/op\n";
}


void benchmarkProcessOrder(int N) {
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(90, 110);

    MockTransactionLogger logTemp;
    TransactionLoggerInterface& log {logTemp};
    OrderBook book;
    MatchingEngine engine(book, log);


    std::vector<Order> buyOrders, sellOrders;
    for (int i {}; i < N; ++i) {
        buyOrders.push_back(Order(Order::OrderType::buy,  dist(rng), 1.0, i, i));
        sellOrders.push_back(Order(Order::OrderType::sell, dist(rng), 1.0, i, i));
    }

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < N; ++i) {
        engine.processOrder(buyOrders[i], sellOrders[i]);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto total = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

    std::cout << "processOrder x" << N
              << " | total: " << total.count() << " ns"
              << " | mean: "  << total.count() / N << " ns/op\n";
}

void benchmarkSimulateMarket(int N) {
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(90, 110);

    MockTransactionLogger logTemp;
    TransactionLoggerInterface& log {logTemp};
    OrderBook book;
    MatchingEngine engine(book,log);

    for (int i {}; i < N; i++) {
        Order order1 {Order(Order::OrderType::buy, dist(rng), 1.0, i, i)};
        Order order2 {Order(Order::OrderType::sell, dist(rng), 1.0, i, i)};
        book.addOrder(order1);
        book.addOrder(order2);
    }

    auto start = std::chrono::high_resolution_clock::now();
    engine.simulateMarket();
    auto end = std::chrono::high_resolution_clock::now();
    auto total = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    
    auto throughput {N / total.count()};

    std::cout << "simulateMarket x" << N
              << " | total: " << total.count() << " ns"
              << " | throughput: "  << throughput << " ns/op\n";
    

}

int main() {

    // Benchmark for one order
    benchmarkAddOrder(1);

    // Benchmark for 1,000 orders
    benchmarkAddOrder(1000);

    // Benchmark for 10,000 orders
    benchmarkAddOrder(10000);

    // Benchmark for 100,000 orders
    benchmarkAddOrder(100000);


    std::cout << std::endl;

    //Benchmark for one order process
    benchmarkProcessOrder(1);

    // Benchmark for 1,000 order processes
    benchmarkProcessOrder(1000);

    // Benchmark for 10,000 order processes
    benchmarkProcessOrder(10000);

    // Benchmark for 100,000 order processes
    benchmarkProcessOrder(100000);

    std::cout << std::endl;

    // Benchmark for market simulation with one order
    benchmarkSimulateMarket(1);

    // Benchmark for market simulation with 1,000 orders
    benchmarkSimulateMarket(1000);

    // Benchark for market simulation with 10,000 orders
    benchmarkSimulateMarket(10000);

    //Benchmark for market simulation with 100,000 orders
    benchmarkSimulateMarket(100000);


    return 0;
}

