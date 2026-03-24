#include <gtest/gtest.h>
#include "Order.h"
#include "OrderBook.h"
#include "MatchingEngine.h"
#include "TransactionLogger.h"
#include <chrono>
#include <thread>
#include <iostream>
#include "sqlite3.h"

TEST(TestTransactionLogger, TestLogTrade) {

    TransactionLogger log {TransactionLogger()};

    Order::OrderType typeBuy = Order::OrderType::buy;
    Order order1 {Order(typeBuy, 20.0, 1.0, 101, 111)};
    Order order2 {Order(typeBuy, 25.0, 2.0, 102, 112)};
    Order order6 {Order{typeBuy, 25.0, 1.0, 106, 116}};

    Order::OrderType typeSell = Order::OrderType::sell;
    Order order3 {Order(typeSell, 20.0, 1.5, 103, 113)};
    Order order4 {Order(typeSell, 15.0, 3.0, 104, 114)};
    Order order5 {Order(typeSell, 20.0, 1.0, 105, 115)};

    OrderBook book1 {OrderBook()};

    book1.addOrder(order1);
    book1.addOrder(order2);
    book1.addOrder(order3);
    book1.addOrder(order4);
    book1.addOrder(order5);
    book1.addOrder(order6);
    int time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    log.logTrade(order1.userId, order3.userId, order1.quantity, order1.price, time);



    sqlite3* db;
    int rc1 = sqlite3_open("../Database/TransactionHistory.db", &db);
    if (rc1 != 0) {
        std::cout << "Error connecting to database: " << sqlite3_errmsg(db) << std::endl;    
        db = nullptr;
        std::runtime_error("Database Connection Failed");
    }
    else {
        std::cout << "Database connection successful!" << std::endl;
    }

    const char* sql = "SELECT * FROM TRADES ORDER BY rowid DESC LIMIT 1;";
    sqlite3_stmt* statement;

    int rc2 = sqlite3_prepare_v2(db, sql, -1, &statement, nullptr);
    int buyUserId;
    int sellUserId;
    int quantity;
    int price;
    int timestamp;

    if (rc2 == SQLITE_OK && sqlite3_step(statement) == SQLITE_ROW) {
        buyUserId = sqlite3_column_int(statement, 0);
        sellUserId = sqlite3_column_int(statement, 1);
        quantity = sqlite3_column_double(statement, 2);
        price = sqlite3_column_double(statement, 3);
        timestamp = sqlite3_column_int(statement, 4);
    }
    else {
        std::cout << "Error " << std::endl;
    }

    
    sqlite3_finalize(statement);

    if (db)
        sqlite3_close(db);

    EXPECT_TRUE(order1.userId == buyUserId && order3.userId == sellUserId && order1.quantity == quantity && order1.price == price && timestamp == time);

    
}