#pragma once
#include "IPriceFetcher.h"
#include <chrono>

class CoinMarketCapFetcher : public IPriceFetcher {
    double cachedPrice = 0.0;
    std::chrono::system_clock::time_point lastFetch{};

    double fetchFromApi();

public:
    double fetchPrice() override;
};
