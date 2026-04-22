#pragma once
#include <sqlite3.h>
#include <cstdint>
#include <string>
#include "TransactionLoggerInterface.h"


class TransactionLogger : public TransactionLoggerInterface{
    sqlite3* db;

    public:
    TransactionLogger(const std::string& dbPath = "Database/TransactionHistory.db");
    ~TransactionLogger();

    // Prevent copying operations
    TransactionLogger(const TransactionLogger&) = delete;
    TransactionLogger& operator=(const TransactionLogger&) = delete;

    // Prevents move assignments
    TransactionLogger(TransactionLogger&&) = delete;
    TransactionLogger& operator=(TransactionLogger&&) = delete;

    void logTrade(int buyUserId, int sellUserId, int64_t quantity, int64_t price, int64_t timestamp) override;
    void bindCheck(sqlite3_stmt* stmt, int returnCode);
};