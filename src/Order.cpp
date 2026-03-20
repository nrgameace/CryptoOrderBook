#include "Order.h"


Order::Order(OrderType side, double priceTemp, double quantityTemp, int transactionIdTemp, int userIdTemp)
        : transactionSide(side), price(priceTemp), quantity(quantityTemp), transactionId(transactionIdTemp), userId(userIdTemp)
    {}

bool Order::operator<(const Order& b) const {
        return timestamp > b.timestamp;
    }

std::chrono::system_clock::time_point Order::getTimestamp() {
        return timestamp;
    }

bool Order::operator==(const Order& other) const {
return price == other.price
    && quantity == other.quantity
    && userId == other.userId
    && transactionId == other.transactionId
    && transactionSide == other.transactionSide;
}