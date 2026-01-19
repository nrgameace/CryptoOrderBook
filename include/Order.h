#pragma once


struct Order {
    public:       
        enum class OrderType {buy, sell};
        OrderType transactionSide; 
        double price;
        double quantity;
    private:
        
        int userId;
        int transactionId;

    public: Order(OrderType side, double priceTemp, double quantityTemp, int userIdTemp, int transactionIdTemp)
        : transactionSide(side), price(priceTemp), quantity(quantityTemp), userId(userIdTemp), transactionId(transactionIdTemp)
    {}

    bool operator<(const Order& b) const {
        return price < b.price;
    }

};