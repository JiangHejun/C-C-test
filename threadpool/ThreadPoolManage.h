/*
 * @Description: interval change time suggest is one long engine recognition time
 * @Author: Hejun Jiang
 * @Date: 2020-11-09 17:11:54
 * @LastEditTime: 2021-02-26 15:48:30
 * @LastEditors: Hejun Jiang
 * @Version: v0.0.1
 * @Contact: jianghejun@hccl.ioa.ac.cn
 * @Corporation: hccl
 */

#ifndef __THREAD_POOL_MANAGE_H__
#define __THREAD_POOL_MANAGE_H__
#include <limits.h>

#include <algorithm>
#include <string>
#include <unordered_set>

#include "ThreadPool.h"

template <typename In>
class ThreadPoolManage {
  public:
    using Engin = std::function<void(In&, int&, std::string&)>;
    explicit ThreadPoolManage(std::unordered_map<std::string, std::pair<Engin, int>> functhreadmap, std::unordered_map<std::string, BlockingQueue<In>*> queuemap, int totalthreadnum,
                              int changeinterval)
        : funcThMap(functhreadmap), totalThNum(totalthreadnum), changeInterval(changeinterval) {
        if (totalThNum < funcThMap.size()) {
            printf("total thread number should more than open engines number\n");
            exit(-1);
        }
        int averge = totalThNum / funcThMap.size(), dvalue = totalThNum % funcThMap.size(), th;
        std::unordered_map<std::string, int> threads;
        std::unordered_set<std::string> rlt;
        for (auto& it : funcThMap) threads[it.first] = averge;
        while (rlt.size() < threads.size()) {
            for (auto& it : threads) {
                if (it.second >= funcThMap[it.first].second) {  // overflow or =
                    dvalue += it.second - funcThMap[it.first].second;
                    it.second = funcThMap[it.first].second;
                    rlt.insert(it.first);
                } else if (dvalue > 0) {  // dvalue
                    it.second++;
                    dvalue--;
                } else if (dvalue <= 0)
                    rlt.insert(it.first);
            }
        }
        for (auto& it : threads) funcMap[it.first] = new ThreadPool<In>(it.second, funcThMap[it.first].second, queuemap[it.first], funcThMap[it.first].first, it.first);

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        int value = 0;
        printf("GetEnableSize: ");
        for (auto& it : funcMap) {
            int v = it.second->GetEnableSize();
            printf("%s[%d], ", it.first.c_str(), v);
            value += v;
        }
        printf("total: %d\n\n", value);
        // printf("GetWorkingSize: ");
        // for (auto& it : funcMap) printf("%s[%d], ", it.first.c_str(), it.second->GetWorkingSize());
        // printf("\n\n");

        if (changeInterval > 0) ch = new std::thread(&ThreadPoolManage::change, this);
    }

    ~ThreadPoolManage() {
        for (auto& it : funcMap) delete it.second;
        if (changeInterval > 0) delete ch;
    }

  private:
    std::unordered_map<std::string, std::pair<Engin, int>> funcThMap;
    int totalThNum;
    int changeInterval;
    std::thread* ch;
    std::unordered_map<std::string, ThreadPool<In>*> funcMap;  // name-thread

    void change() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(changeInterval));
            ReverseThreadAvg();
            // ReverseThreadNum();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            int value = 0;
            printf("GetEnableSize: ");
            for (auto& it : funcMap) {
                int v = it.second->GetEnableSize();
                printf("%s[%d], ", it.first.c_str(), v);
                value += v;
            }
            printf("total: %d\n\n", value);
            // printf("GetWorkingSize: ");
            // for (auto& it : funcMap) printf("%s[%d], ", it.first.c_str(), it.second->GetWorkingSize());
            // printf("\n\n");
        }
    }

  private:
    bool cmp(std::pair<std::string, long long> x, std::pair<std::string, long long> y) { return x.second > y.second; }

    void ReverseThreadAvg() {
        int nowTNum = totalThNum;
        std::unordered_map<std::string, long long> done;
        long long donet = 0;
        printf("now done: ");
        for (auto& it : funcMap) {
            std::unique_lock<std::mutex> numlock(it.second->nummtx);
            if (it.second->bque->empty()) {
                it.second->ChangeEnable(1);
                nowTNum--;
                printf("%s[%lld], ", it.first.c_str(), it.second->donenum);
                it.second->donenum = 0;  // clear 0
                continue;
            }
            done[it.first] = it.second->donenum ? it.second->donenum : 1;  // it.second->donenum!=0
            donet += done[it.first];
            it.second->donenum = 0;  // clear 0
            printf("%s[%lld], ", it.first.c_str(), done[it.first]);
        }
        printf("\n");

        double doneavg = (double)donet / done.size(), totalrate = 0;
        std::unordered_map<std::string, double> donerate;
        for (auto& it : done) {
            donerate[it.first] = doneavg / it.second * funcMap[it.first]->GetEnableSize();
            totalrate += donerate[it.first];
        }
        printf("the avg donerate: ");
        for (auto& it : donerate) printf("%s[%.2f], ", it.first.c_str(), it.second);
        printf("\n");

        std::unordered_map<std::string, int> threads;
        int total = 0;
        for (auto& it : donerate) {
            threads[it.first] = it.second / totalrate * nowTNum ? it.second / totalrate * nowTNum : 1;  // can not =0
            total += threads[it.first];
        }
        printf("the avg threads: ");
        for (auto& it : threads) printf("%s[%d], ", it.first.c_str(), it.second);
        printf("\n");

        int overflow = nowTNum - total;  // may>0, may<0
        for (auto& it : threads) {       // 2,8,5,3 --6--> 2,6,6,4
            if (it.second > funcThMap[it.first].second) {
                overflow += (it.second - funcThMap[it.first].second);
                it.second = funcThMap[it.first].second;
            }
        }
        while (true) {  // for more times
            bool isfull = true;
            for (auto& it : threads) {
                if (overflow > 0 && it.second < funcThMap[it.first].second) {
                    it.second++;
                    overflow--;
                    isfull = false;
                } else if (overflow < 0 && it.second > 1) {  // it.second at least =1
                    it.second--;
                    overflow++;
                    isfull = false;
                }
            }
            if (isfull) break;
        }
        printf("the avg threads done: ");
        for (auto& it : threads) printf("%s[%d], ", it.first.c_str(), it.second);
        printf("\n");

        for (auto& it : threads) funcMap[it.first]->ChangeEnable(it.second);
    }

    void ReverseThreadNum() {
        // now done: bamp[312], lid[90], sid[78], cn[61], arab[61], tur[61], tib[61], uig[61],
        printf("now done: ");
        std::unordered_set<long long> doneset;  // donenum
        std::unordered_map<std::string, long long> done;
        for (auto& it : funcMap) {
            long long v = it.second->donenum ? it.second->donenum : 1;  // it.second->donenum!=0
            it.second->donenum = 0;                                     // clear 0
            doneset.insert(v);
            done[it.first] = v;
            printf("%s[%lld], ", it.first.c_str(), done[it.first]);
        }
        printf("\n");

        // done set: 61, 78, 90, 312
        // tochange: bamp[61], uig[312], tib[312], tur[312], arab[312], cn[312], lid[78], sid[90],
        printf("now done reverse: ");
        std::vector<std::pair<std::string, long long>> reverseDone;
        for (size_t n = 0; n < doneset.size() / 2; n++) {
            long long max = INT64_MIN, min = INT64_MAX;
            for (auto& it : done) {
                if (it.second != INT64_MIN && it.second >= max) max = it.second;
                if (it.second != INT64_MIN && it.second <= min) min = it.second;
            }
            std::vector<std::pair<std::string, long long>> maxitvec, minitvec;
            for (auto& it : done) {
                if (it.second == max) {
                    maxitvec.emplace_back(std::make_pair(it.first, min));
                    it.second = INT64_MIN;
                }
                if (it.second == min) {
                    minitvec.emplace_back(std::make_pair(it.first, max));
                    it.second = INT64_MIN;
                }
            }
            for (auto& it : maxitvec) {
                reverseDone.emplace_back(it);
                printf("%s[%lld], ", reverseDone.back().first.c_str(), reverseDone.back().second);
            }
            for (auto& it : minitvec) {
                reverseDone.emplace_back(it);
                printf("%s[%lld], ", reverseDone.back().first.c_str(), reverseDone.back().second);
            }
        }
        if (doneset.size() % 2) {
            for (auto& it : done)
                if (it.second != INT64_MIN) reverseDone.emplace_back(std::make_pair(it.first, it.second));
        }
        printf("\n");

        // now done reverse sorted: uig[312], tib[312], tur[312], arab[312], cn[312], sid[90], lid[78], bamp[61],
        std::sort(reverseDone.begin(), reverseDone.end(), std::bind(&ThreadPoolManage::cmp, this, std::placeholders::_1,
                                                                    std::placeholders::_2));  // min-->max
        printf("now done reverse sorted: ");
        long long total = 0;
        for (auto& d : reverseDone) {
            total += d.second;
            printf("%s[%lld], ", d.first.c_str(), d.second);
        }
        printf("\n");

        // rate reverse sorted: uig[0.17], tib[0.17], tur[0.17], arab[0.17], cn[0.17], sid[0.05], lid[0.04], bamp[0.03],
        printf("rate reverse sorted: ");
        std::vector<std::pair<std::string, double>> rt;
        for (auto& it : reverseDone) {
            auto v = ((double)it.second) / total;
            rt.emplace_back(std::make_pair(it.first, v));
            printf("%s[%.2f], ", it.first.c_str(), v);
        }
        printf("\n");

        // thread number sorted: uig[5], tib[5], tur[5], arab[5], cn[5], sid[1], lid[1], bamp[1], =28 <30
        printf("thread number sorted: ");
        std::vector<std::pair<std::string, int>> threads;
        int tmp = 0;
        for (auto it = rt.begin(); it != rt.end(); it++) {
            int th = it->second * totalThNum;
            tmp += th;
            threads.emplace_back(std::make_pair(it->first, th));
            printf("%s[%d], ", threads.back().first.c_str(), threads.back().second);
        }
        printf("\n");

        // thread number sorted revised: uig[6], tib[6], tur[5], arab[5], cn[5], sid[1], lid[1], bamp[1],
        int overflow = totalThNum - tmp;
        for (auto& it : threads) {  // 2,8,5,3 --6--> 2,6,6,4
            if (it.second > funcThMap[it.first].second) {
                overflow += (it.second - funcThMap[it.first].second);
                it.second = funcThMap[it.first].second;
            }
        }
        while (true) {
            bool isfull = true;
            for (auto& it : threads) {
                if (it.second < funcThMap[it.first].second && overflow > 0) {
                    it.second++;
                    overflow--;
                    isfull = false;
                }
            }
            if (isfull) break;
        }

        printf("thread number sorted revised: ");
        for (auto& it : threads) printf("%s[%d], ", it.first.c_str(), it.second);
        printf("\n");

        for (auto& it : threads) funcMap[it.first]->ChangeEnable(it.second);
    }
};

#endif
