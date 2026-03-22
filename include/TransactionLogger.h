#pragma once
#include <sqlite3.h>
#include <cstdint>


class TransactionLogger {
    sqlite3* db;

    public:
    TransactionLogger();
    ~TransactionLogger();
    void logTrade(int buyUserId, int sellUserId, int64_t quantity, int64_t price, int timestamp);
};