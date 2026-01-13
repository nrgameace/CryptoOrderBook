#pragma once

#include <string>

struct Order {
    public: 
        double price;
        std::string orderType;
        bool buyOffer;
    private:
        int userId;

    public: Order(double price, const std::string& orderType, bool buyOffer, int userID)
    {
        this->price = price;
        this->orderType = orderType;
        this->buyOffer = buyOffer;
        this->userId = userID;
    }
};