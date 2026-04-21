#pragma once

class IPriceFetcher {
public:
    virtual ~IPriceFetcher() = default;
    virtual double fetchPrice() = 0;
};
