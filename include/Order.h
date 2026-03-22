#pragma once
#include <ctime>
#include <chrono>
#include "Utils.h"

struct Order {
    public:
        enum class OrderType {buy, sell};
        OrderType transactionSide;
        int64_t price;
        int64_t quantity;
        std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now();
        int transactionId;
        int userId;
    
        
    
    public: 
        Order(OrderType side, double priceTemp, double quantityTemp, int transactionIdTemp, int userIdTemp);
        bool operator<(const Order& b) const;
        std::chrono::system_clock::time_point getTimestamp();
        bool operator==(const Order& other) const;

};