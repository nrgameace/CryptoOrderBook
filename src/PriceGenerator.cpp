#include "PriceGenerator.h"
#include "httplib.h"
#include "dotenv.h"
#include <nlohmann/json.hpp>
#include "Order.h"
#include "Utils.h"
#include <vector>
#include <stdexcept>
#include <iostream>
#include <string>
#include <cstdlib>
#include <random>




PriceGenerator::PriceGenerator() 
    :currentPrice(0.0), orders(std::vector<Order>()) {}


void PriceGenerator::updatePrice() {
    httplib::Client cli("https://pro-api.coinmarketcap.com");

    dotenv::init("../.env"); 

    const char* apiKey = std::getenv("COIN_MARKET_API_KEY");

    if (!apiKey) {
        throw std::runtime_error("API Key Not Loaded");
    }

    httplib::Headers headers = {
      {"X-CMC_PRO_API_KEY", apiKey},
    };

    auto response = cli.Get("/v1/cryptocurrency/listings/latest?symbol=BTC", headers);

    if (response && response->status == 200) {
        auto json = nlohmann::json::parse(response->body);
        auto currentPriceTemp = json["data"][0]["quote"]["USD"]["price"];

        currentPrice = currentPriceTemp;
    }
    else {
        std::cout << response->status << std::endl;
        throw std::runtime_error("Error fetching prices from API");
    }

}

std::vector<Order> PriceGenerator::getOrders() {
    return orders;
}

void PriceGenerator::generateOrders(int numOrders) {
    std::random_device seed;
    std::mt19937 gen(seed());

	updatePrice();
	
    std::normal_distribution<double> priceDist(currentPrice, 15);
    std::uniform_int_distribution<int> sideDist{0, 1};



    for (int i {}; i < numOrders; i++) {
        int priceTemp = priceDist(seed);
        int randomSide = sideDist(seed);

        Order::OrderType side = Order::OrderType::buy;
        double quantity = 2.0;

        if (randomSide == 1)
          side = Order::OrderType::sell;

        Order temp {Order(side, priceTemp, quantity, 1, 1)};
		orders.push_back(temp);
    }

}
