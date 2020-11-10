#include <stdio.h>

#include <string>
#include <thread>

#include "BlockingQueue.h"

void PushQue(BlockingQueue<std::string>* que, int n) {
    for (int i = 0; i < 100; i++) que->push(std::to_string(n * 100 + i) + "_test");
}

void ShowQue(BlockingQueue<std::string>* que) {
    while (true) {
        printf("bque item[%s], bque now len[%lu]\n", que->pop().c_str(), que->size());
    }
}

int main() {
    BlockingQueue<std::string> bque;
    std::thread pop(ShowQue, &bque);
    std::thread* push = new std::thread[10];
    for (int i = 0; i < 1; i++) push[i] = std::thread(PushQue, &bque, i);
    for (int i = 0; i < 1; i++) push[i].join();
    pop.join();
    return 0;
}