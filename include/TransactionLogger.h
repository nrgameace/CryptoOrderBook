#include <sqlite3.h>


class TransactionLogger {
     sqlite3* db;

     TransactionLogger(sqlite3* db);
     void logTrade(int buyUserId, int sellUserId, double quantity, double price, int timestamp);
};