#include "Order.h"


Order::Order(OrderType side, double priceTemp, double quantityTemp, int transactionIdTemp, int userIdTemp)
        : transactionSide(side), price(convertToInternal(priceTemp)), quantity(convertToInternal(quantityTemp)), transactionId(transactionIdTemp), userId(userIdTemp)
    {}

bool Order::operator<(const Order& b) const {
        return timestamp < b.timestamp;
    }

std::chrono::system_clock::time_point Order::getTimestamp() const {
        return timestamp;
    }

bool Order::operator==(const Order& other) const {
return price == other.price
    && quantity == other.quantity
    && userId == other.userId
    && transactionId == other.transactionId
    && transactionSide == other.transactionSide;
}