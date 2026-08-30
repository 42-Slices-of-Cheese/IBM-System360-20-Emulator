#include <string>
#include <cstdint>

#include "helpers.hpp"


int32_t removeFirstNDigits(int32_t number, uint8_t n)
{
    std::string str = std::to_string(number);

    // Handle negative numbers
    bool isNegative = false;
    if (str[0] == '-') {
        isNegative = true;
        str.erase(0, 1); // remove minus sign temporarily
    }

    if (n >= str.length()) {
        return 0; // If removing all digits or more, return 0
    }

    // Remove first n digits
    str.erase(0, n);

    int32_t result = std::stoll(str);

    return isNegative ? -result : result;
}