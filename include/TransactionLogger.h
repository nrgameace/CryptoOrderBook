#pragma once
#include <sqlite3.h>


class TransactionLogger {
    sqlite3* db;
    
    public:
    TransactionLogger();
    ~TransactionLogger();
    void logTrade(int buyUserId, int sellUserId, double quantity, double price, int timestamp);
};