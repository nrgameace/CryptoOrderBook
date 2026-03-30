#pragma once
#include <cstdint>
#include <vector>
#include "TransactionLoggerInterface.h"
#include "TradeRecord.h"

class TestTransactionLogger : TransactionLoggerInterface {
    std::vector<TradeRecord> log;

    public:
        
};