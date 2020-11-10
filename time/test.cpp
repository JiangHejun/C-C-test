// #include <time.h>

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>

int main() {
    // auto tp = std::chrono::steady_clock::now();
    // auto tp = std::chrono::system_clock::now();
    // auto cur_time = std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch()).count();
    auto s = std::chrono::steady_clock::now();
    auto cur_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string str_time = std::ctime(&cur_time);
    std::cout << str_time << std::endl;
    cur_time += 60 * 60 * 24;
    str_time = std::ctime(&cur_time);
    std::cout << str_time << std::endl;
    auto e = std::chrono::steady_clock::now();
    std::cout << "spend time ns " << std::chrono::duration_cast<std::chrono::nanoseconds>(e - s).count() << std::endl;
    std::cout << "spend time us " << std::chrono::duration_cast<std::chrono::microseconds>(e - s).count() << std::endl;
    std::cout << "spend time ms " << std::chrono::duration_cast<std::chrono::milliseconds>(e - s).count() << std::endl;
    return 0;
}