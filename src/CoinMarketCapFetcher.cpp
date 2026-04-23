#include "CoinMarketCapFetcher.h"
#include "httplib.h"
#include "dotenv.h"
#include <nlohmann/json.hpp>
#include <cstdlib>
#include <stdexcept>
#include <iostream>

double CoinMarketCapFetcher::fetchFromApi() {
    httplib::Client cli("https://pro-api.coinmarketcap.com");
    dotenv::init("../.env");

    const char* apiKey = std::getenv("COIN_MARKET_API_KEY");
    if (!apiKey) {
        throw std::runtime_error("API Key Not Loaded");
    }

    httplib::Headers headers = {{"X-CMC_PRO_API_KEY", apiKey}};
    auto response = cli.Get("/v1/cryptocurrency/listings/latest?symbol=BTC", headers);

    if (!response) {
        throw std::runtime_error("CMC Request Failed");
    }

    else if (response && response->status == 200) {
        auto json = nlohmann::json::parse(response->body);
        return json["data"][0]["quote"]["USD"]["price"];
    }

    std::cout << response->status << std::endl;
    throw std::runtime_error("Error fetching prices from API");
}

double CoinMarketCapFetcher::fetchPrice() {
    auto now = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = now - lastFetch;

    if (elapsed.count() >= 1.0) {
        cachedPrice = fetchFromApi();
        lastFetch = now;
    }

    return cachedPrice;
}
