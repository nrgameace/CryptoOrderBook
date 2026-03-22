#pragma once
#include <cstdint>
#include <cmath>

constexpr int64_t SCALE_FACTOR = 100000000;

int64_t convertToInternal(double value);
double convertToDouble(int64_t value);
