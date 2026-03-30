#pragma once
#include <cstdint>
#include <vector>
#include "TransactionLoggerInterface.h"
#include "TradeRecord.h"

class MockTransactionLogger : public TransactionLoggerInterface {
    std::vector<TradeRecord> log;

    public:
        MockTransactionLogger();
        void logTrade(int buyUserId, int sellUserId, int64_t quantity, int64_t price, int timestamp);
        TradeRecord getRecord(int index);

};