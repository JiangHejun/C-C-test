#include <chrono>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>

#include "BlockingQueue.h"
#include "ThreadPoolManage.h"

using In = int;

void bampengin(In& a, int& b, std::string& s) {
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // while (ptr->GePermission(n)) {
    // ptr->GePermission(std::this_thread::get_id());
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // printf("intput[%d]\n", a);
    // break;
    // std::cout << std::this_thread::get_id() << std::endl;
    // }
}

void lidengin(In& a, int& b, std::string& s) {
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // while (ptr->GePermission(n)) {
    // ptr->GePermission(std::this_thread::get_id());
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    // printf("intput[%d]\n", a);
    // printf("thread id[%d]\n", n);
    // break;
    // std::cout << std::this_thread::get_id() << std::endl;
    // }
}

void sidengin(In& a, int& b, std::string& s) {
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // while (ptr->GePermission(n)) {
    // ptr->GePermission(std::this_thread::get_id());
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    // printf("intput[%d]\n", a);
    // printf("thread id[%d]\n", n);
    // break;
    // std::cout << std::this_thread::get_id() << std::endl;
    // }
}

void kwengin(In& a, int& b, std::string& s) {
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // while (ptr->GePermission(n)) {
    // ptr->GePermission(std::this_thread::get_id());
    std::this_thread::sleep_for(std::chrono::seconds(1));
    // printf("intput[%d]\n", a);
    // printf("thread id[%d]\n", n);
    // break;
    // std::cout << std::this_thread::get_id() << std::endl;
    // }
}

void pushque(BlockingQueue<In>* bque) {
    int i = 0;
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        bque->push(i++);
    }
}

int main() {
    std::unordered_map<std::string, BlockingQueue<In>*> quemap;
    quemap["bamp"] = new BlockingQueue<In>;
    auto Push1 = new std::thread(pushque, quemap["bamp"]);
    quemap["lid"] = new BlockingQueue<In>;
    auto Push2 = new std::thread(pushque, quemap["lid"]);
    quemap["sid"] = new BlockingQueue<In>;
    auto Push3 = new std::thread(pushque, quemap["sid"]);
    quemap["cn"] = new BlockingQueue<In>;
    auto Push4 = new std::thread(pushque, quemap["cn"]);
    quemap["arab"] = new BlockingQueue<In>;
    auto Push5 = new std::thread(pushque, quemap["arab"]);
    quemap["tur"] = new BlockingQueue<In>;
    auto Push6 = new std::thread(pushque, quemap["tur"]);
    quemap["tib"] = new BlockingQueue<In>;
    auto Push7 = new std::thread(pushque, quemap["tib"]);
    quemap["uig"] = new BlockingQueue<In>;
    auto Push8 = new std::thread(pushque, quemap["uig"]);

    int totalThNum = 30, checkInter = 10;
    std::unordered_map<std::string, std::pair<std::function<void(In&, int&, std::string&)>, int>> ftm;
    ftm["bamp"] = std::make_pair(std::bind(bampengin, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), 30);
    ftm["lid"] = std::make_pair(std::bind(lidengin, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), 1);
    ftm["sid"] = std::make_pair(std::bind(sidengin, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), 30);
    ftm["cn"] = std::make_pair(std::bind(kwengin, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), 30);
    ftm["arab"] = std::make_pair(std::bind(kwengin, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), 30);
    ftm["tur"] = std::make_pair(std::bind(kwengin, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), 30);
    ftm["tib"] = std::make_pair(std::bind(kwengin, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), 30);
    ftm["uig"] = std::make_pair(std::bind(kwengin, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), 30);
    ThreadPoolManage<In> manage(ftm, quemap, totalThNum, checkInter);

    Push1->join();
    return 0;
}