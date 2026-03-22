#include "Utils.h"

int64_t convertToInternal(double value) {
    return static_cast<int64_t>(std::llround(value * SCALE_FACTOR));
}

double convertToDouble(int64_t value) {
    return static_cast<double>(value) / SCALE_FACTOR;
}
