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
    quemap["kw"] = new BlockingQueue<In>;
    auto Push4 = new std::thread(pushque, quemap["kw"]);

    int totalThNum = 30, checkInter = 1;
    std::unordered_map<std::string, std::pair<std::function<void(In&, int&, std::string&)>, int>> ftm;
    ftm["bamp"] = std::make_pair(std::bind(bampengin, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), 30);
    ftm["lid"] = std::make_pair(std::bind(lidengin, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), 30);
    ftm["sid"] = std::make_pair(std::bind(sidengin, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), 30);
    ftm["kw"] = std::make_pair(std::bind(kwengin, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), 30);
    ThreadPoolManage<In> manage(ftm, quemap, totalThNum, checkInter);

    Push1->join();
    delete Push1;
    delete Push2;
    delete Push3;
    delete Push4;
    return 0;
}