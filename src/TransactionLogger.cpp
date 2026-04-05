#include <sqlite3.h>
#include "TransactionLogger.h"
#include "Utils.h"
#include <stdexcept>
#include <string>



TransactionLogger::TransactionLogger() {
    int rc = sqlite3_open("../Database/TransactionHistory.db", &db);
    if (rc != 0) {
        throw std::runtime_error(std::string("Error connecting to database: ") + sqlite3_errmsg(db));    
        db = nullptr;
        throw std::runtime_error("Database Connection Failed");
    }
}


TransactionLogger::~TransactionLogger() {
    if (db) {
        sqlite3_close(db);
    }
}

/**
 * @brief Method that logs a transaction in the TRADES database
 * @param buyUserId The buying user's ID pertaining to that transaction
 * @param sellUserId The selling user's ID that pertains to that transaction
 * @param quantity The amount that was agreed upon in the trade
 * @param price The agreed upon price
 * @param timestamp The time at which the transaction occurred
 */
void TransactionLogger::logTrade(int buyUserId, int sellUserId, int64_t quantity, int64_t price, int timestamp) {
    const char* sql = "INSERT INTO TRADES (buyUserId, sellUserId, quantity, price, timestamp) VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;

    // 2. Prepare the SQL statement into a prepared statement object (sqlite3_stmt)
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        throw std::runtime_error(sqlite3_errmsg(db));
    }

    // 3. Bind C++ variables to the placeholders
    // The first parameter '1' refers to the first '?'
    bindCheck(stmt, sqlite3_bind_int(stmt, 1, buyUserId)); 
    bindCheck(stmt, sqlite3_bind_int(stmt, 2, sellUserId));
    bindCheck(stmt, sqlite3_bind_double(stmt, 3, convertToDouble(quantity)));
    bindCheck(stmt, sqlite3_bind_double(stmt, 4, convertToDouble(price)));
    bindCheck(stmt, sqlite3_bind_int(stmt, 5, timestamp));

    // 4. Execute the statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        throw std::runtime_error(sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);

}


void TransactionLogger::bindCheck(sqlite3_stmt* stmt, int returnCode) {
    if (returnCode != SQLITE_OK) {
        sqlite3_finalize(stmt);
        throw std::runtime_error(sqlite3_errmsg(db));
    }
}