#pragma once

#include <string>

struct Order {
    public:       
        enum class OrderType {buy, sell};
        OrderType transactionSide; 
        double price;
        double quantity;
    private:
        
        int userId;
        int transactionId;

    public: Order(OrderType side, double priceTemp, double quantityTemp, int transactionIdTemp, int userIdTemp)
        : transactionSide(side), price(priceTemp), quantity(quantityTemp), transactionId(transactionIdTemp), userId(userIdTemp)
    {

    }
};