#pragma once
#include <cstdint>


class TransactionLoggerInterface {
    public:
        virtual ~TransactionLoggerInterface() = default;
        virtual void logTrade(int buyUserId, int sellUserId, int64_t quantity, int64_t price, int64_t timestamp) = 0;
};