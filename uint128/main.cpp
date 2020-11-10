#include <stdint.h>

#include <iostream>
#include <sstream>
#include <string>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
#define OS_WIN
#endif

#ifndef OS_WIN
#define UINT128 __uint128_t  // 0xffffffffffffffffffffffffffffffffui128
#else
#define UINT128 uint64_t  // 0xffffffffffffffffui64
#endif

std::string u128toa(UINT128 v) {
    std::string s, str;
    while (v > 0) {
        s.push_back(v % 10 + '0');
        v /= 10;
    }
    int len = s.length();
    for (int i = len - 1; i >= 0; i--) str.push_back(s[i]);
    return str;
}

int main() {
    UINT128 a = 0xffffffffffffffffffffffffffffffff;
    uint64_t b = 0xffffffffffffffff;
    UINT128 x = 1100000000000000L;
    UINT128 y = 2200000000000000L;

    std::cout << u128toa(a) << std::endl;

    x *= y;
    std::cout << u128toa(x) << std::endl;

    std::stringstream bb;
    bb << b;
    std::cout << bb.str() << std::endl;
    return 0;
}