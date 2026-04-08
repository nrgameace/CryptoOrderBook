#pragma once
#include "httplib.h"
#include "Order.h"
#include <vector>

class PriceGenerator {

    private:
        int64_t currentPrice;
        std::vector<Order> orders;

    public:
        PriceGenerator();
        void updatePrice();
        std::vector<Order> getOrders();
        void generateOrders();


};