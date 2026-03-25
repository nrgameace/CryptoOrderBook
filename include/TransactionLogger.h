#pragma once
#include <sqlite3.h>
#include <cstdint>


class TransactionLogger {
    sqlite3* db;

    public:
    TransactionLogger();
    ~TransactionLogger();

    // Prevent copying operations
    TransactionLogger(const TransactionLogger&) = delete;
    TransactionLogger& operator=(const TransactionLogger&) = delete;

    // Prevents move assignments
    TransactionLogger(TransactionLogger&&) = delete;
    TransactionLogger& operator=(TransactionLogger&&) = delete;

    void logTrade(int buyUserId, int sellUserId, int64_t quantity, int64_t price, int timestamp);
    void bindCheck(sqlite3_stmt* stmt, int returnCode);
};