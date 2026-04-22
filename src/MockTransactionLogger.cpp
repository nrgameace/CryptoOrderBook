#include "MockTransactionLogger.h"
#include "TradeRecord.h"
#include <vector>


MockTransactionLogger::MockTransactionLogger() {
    log = std::vector<TradeRecord>();
}

void MockTransactionLogger::logTrade(int buyUserId, int sellUserId, int64_t quantity, int64_t price, int64_t timestamp) {
    TradeRecord temp {TradeRecord(buyUserId, sellUserId, quantity, price, timestamp)};
    log.push_back(temp);
}

TradeRecord MockTransactionLogger::getRecord(int index) {
    return log.at(index);
}