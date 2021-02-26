/*
 * @Description: threadpool for one engin
 * @Author: Hejun Jiang
 * @Date: 2020-11-06 15:38:31
 * @LastEditTime: 2021-02-26 15:47:19
 * @LastEditors: Hejun Jiang
 * @Version: v0.0.1
 * @Contact: jianghejun@hccl.ioa.ac.cn
 * @Corporation: hccl
 */
#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__
#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "BlockingQueue.h"

template <typename In>
class ThreadPool {
  public:
    using Engin = std::function<void(In&, int&, std::string&)>;
    explicit ThreadPool(int threadnum, int maxthreadnum, BlockingQueue<In>* queue, const Engin enginefunc, std::string name) : donenum(0), bque(queue), engine(enginefunc) {
        for (int i = 0; i < maxthreadnum; i++) {
            if (i < threadnum)
                enable[i] = true;
            else
                enable[i] = false;
            // waiting[i] = false;
            encond[i] = new std::condition_variable;
            // wacond[i] = new std::condition_variable;
            funcVec.push_back(new std::thread(&ThreadPool::run, this, i, name));
        }
        printf("name[%s], funcVecsize[%lu], enablesize[%lu], encondsize[%lu]\n", name.c_str(), funcVec.size(), enable.size(), encond.size());
        // printf("funcVecsize[%lu], enablesize[%lu], waitingsize[%lu], encondsize[%lu], wacondsize[%lu]\n",
        // funcVec.size(), enable.size(), waiting.size(), encond.size(), wacond.size());
    }

    ~ThreadPool() {
        for (auto& t : funcVec)
            if (t->joinable()) t->join();
    }

    void ChangeEnable(int ennum) {
        std::unique_lock<std::mutex> lock(mtx);
        int tnum = 0;  // now enable
        for (auto& it : enable) tnum += it.second;
        int enablenum = ennum - tnum;  // changenum - now enable
        // printf("%d=%d-%d\n", enablenum, ennum, tnum);
        if (enablenum > 0) {  // add enable
            for (auto& it : enable) {
                if (!it.second) {  // enable=false, waiting=true
                    // if (!waiting[it.first]) wacond[it.first]->wait(lock);  // waiting=false, running in
                    // last time
                    encond[it.first]->notify_one();  // notity to the waiting thread, not waiting won't get the notify
                    if (--enablenum == 0) break;
                }
            }
        } else if (enablenum < 0) {  // decrease enable
            for (auto& it : enable) {
                if (it.second) {  // enable=true, waiting=true
                    // if (!waiting[it.first]) wacond[it.first]->wait(lock);  // waiting=false, running in
                    // last time
                    it.second = false;
                    if (++enablenum == 0) break;
                }
            }
        }
    }

    int GetEnableSize() {
        std::unique_lock<std::mutex> lock(mtx);
        int n = 0;
        for (auto& it : enable) n += it.second;
        return n;
    }

    // int GetWorkingSize() {
    //     std::unique_lock<std::mutex> lock(mtx);
    //     int n = 0;
    //     for (auto& it : waiting) n += it.second;
    //     return waiting.size() - n;
    // }

    std::mutex nummtx;
    long long donenum;
    BlockingQueue<In>* bque;

  private:
    std::vector<std::thread*> funcVec;
    Engin engine;
    std::mutex mtx;
    std::unordered_map<int, bool> enable;  // mutex
    // std::unordered_map<int, bool> waiting;  // mutex
    // std::unordered_map<int, std::condition_variable*> encond, wacond;  // mutex
    std::unordered_map<int, std::condition_variable*> encond;  // mutex

    bool getPermission(int tid) {
        std::unique_lock<std::mutex> lock(mtx);
        // waiting[tid] = true;  // ensure the tid is waiting
        // wacond[tid]->notify_one();  // release the changeenable waiting
        if (!enable[tid]) {
            encond[tid]->wait(lock);          // sleeping
            enable.find(tid)->second = true;  // make the tid enable true
        }
        // waiting[tid] = false;  // ensure the tis is not waiting
        return true;
    }

    void run(int n, std::string name) {  // this is each thread
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        while (this->getPermission(n)) {
            auto data = bque->pop();
            this->engine(data, n, name);
            std::unique_lock<std::mutex> numlock(nummtx);
            donenum++;
        }
    }
};

#endif