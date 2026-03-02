#include <sqlite3.h>
#include "TransactionLogger.h"
#include <iostream>



TransactionLogger::TransactionLogger(sqlite3* db) {
    int rc = sqlite3_open("Database/TransactionHistory.db", &db);
    if (rc != 0) {
        std::cout << "Error connecting to database: " << sqlite3_errmsg(db) << std::endl;    
        db = nullptr;
    }
    else {
        std::cout << "Database connection successful!" << std::endl;
    }
}


void TransactionLogger::logTrade(int buyUserId, int sellUserId, double quantity, double price, int timestamp) {
    const char* sql = "INSERT INTO TRADES (buyUserId, sellUserId, quantity, price, timestamp) VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;

    // 2. Prepare the SQL statement into a prepared statement object (sqlite3_stmt)
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    // 3. Bind C++ variables to the placeholders
    // The first parameter '1' refers to the first '?'
    sqlite3_bind_int(stmt, 1, buyUserId); 
    sqlite3_bind_int(stmt, 2, sellUserId);
    sqlite3_bind_double(stmt, 3, quantity);
    sqlite3_bind_double(stmt, 4, price);
    sqlite3_bind_int(stmt, 5, timestamp);

    // 4. Execute the statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Execution failed: " << sqlite3_errmsg(db) << std::endl;
    } else {
        std::cout << "Row inserted successfully." << std::endl;
    }

    sqlite3_finalize(stmt);

}
