#include <gtest/gtest.h>
#include "TradeRecord.h"
#include "MockTransactionLogger.h"


TEST(TestTransactionLogger, TestLogTrade) {

    TradeRecord record {TradeRecord(123, 234, 2, 19, 123456)};
    TradeRecord falseRecord {TradeRecord(124, 234, 3, 20, 12345)};
    
    MockTransactionLogger logger {MockTransactionLogger()};

    logger.logTrade(record.buyUserId, record.sellUserId, record.quantity, record.price, record.timestamp);
    TradeRecord temp = logger.getRecord(0);

    EXPECT_EQ(record, temp);
    EXPECT_NE(temp, falseRecord);
    
}