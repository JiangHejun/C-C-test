#include <stdio.h>

#include <chrono>
#include <iostream>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

std::map<std::string, std::mutex*> mtx;
std::map<std::string, std::vector<std::string>> m;

void t1() {
    int i = 0;
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        std::unique_lock<std::mutex> lock(*mtx["test1"]);
        m["test1"].push_back(std::to_string(i++) + "_test1");
        // std::cout << "test1 size " << m["test1"].size() << std::endl;
    }
}

void t2() {
    int i = 0;
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        std::unique_lock<std::mutex> lock(*mtx["test2"]);
        m["test2"].push_back(std::to_string(i++) + "_test2");
        // std::cout << "test2 size " << m["test2"].size() << std::endl;
    }
}
void t3() {
    int i = 0;
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        std::unique_lock<std::mutex> lock(*mtx["test3"]);
        m["test3"].push_back(std::to_string(i++) + "_test3");
        std::cout << "test3 size " << m["test3"].size() << std::endl;
    }
}

int main() {
    // std::map<std::string, int> map;
    // map["jhj"] = 100;
    // printf("%d\n", map["jhj"]);
    // map["jhj"] = 99;
    // printf("%d\n", map["jhj"]);
    // map["hwq"] = 10;
    // printf("%d\n", map["hwq"]);
    // printf("%d\n", map["jjj"]);
    // printf("%lu\n", map.size());

    m["test1"] = std::vector<std::string>();
    m["test2"] = std::vector<std::string>();
    m["test3"] = std::vector<std::string>();
    mtx["test1"] = new std::mutex;
    mtx["test2"] = new std::mutex;
    mtx["test3"] = new std::mutex;

    auto p1 = new std::thread(t1);
    auto p2 = new std::thread(t2);
    auto p3 = new std::thread(t3);

    p1->join();
    return 0;
}