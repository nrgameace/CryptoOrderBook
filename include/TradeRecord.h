#pragma once
#include <cstdint>


struct TradeRecord {
    int buyUserId;
    int sellUserId;
    int64_t quantity;
    int64_t price;
    int timestamp;

    TradeRecord(int buyUserIdTemp, int sellUserIdTemp, int64_t quantityTemp, int64_t priceTemp, int timestampTemp)
    : buyUserId(buyUserIdTemp), sellUserId(sellUserIdTemp), quantity(quantityTemp), price(priceTemp), timestamp(timestampTemp)
    {}

    bool operator==(const TradeRecord& other) const {
        return buyUserId == other.buyUserId && sellUserId == other.sellUserId && quantity == other.quantity && price == other.price && timestamp == other.timestamp;
    }

};