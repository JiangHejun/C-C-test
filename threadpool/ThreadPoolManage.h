/*
 * @Description: interval change time suggest is one long engine recognition time
 * @Author: Hejun Jiang
 * @Date: 2020-11-09 17:11:54
 * @LastEditTime: 2020-11-12 16:46:03
 * @LastEditors: Hejun Jiang
 * @Version: v0.0.1
 * @Contact: jianghejun@hccl.ioa.ac.cn
 * @Corporation: hccl
 */

#ifndef __THREAD_POOL_MANAGE_H__
#define __THREAD_POOL_MANAGE_H__
#include <algorithm>
#include <string>

#include "ThreadPool.h"

template <typename In, typename Out>
class ThreadPoolManage {
  public:
    using Engin = std::function<void(In&, Out&)>;
    explicit ThreadPoolManage(std::unordered_map<std::string, std::pair<Engin, int>> functhreadmap, std::unordered_map<std::string, BlockingQueue<In>*> queuemap, int totalthreadnum,
                              int changeinterval)
        : totalThNum(totalthreadnum), changeInterval(changeinterval), funcThMap(functhreadmap) {
        int averge = totalThNum / funcThMap.size(), dvalue = totalThNum % funcThMap.size(), th;
        for (auto& it : funcThMap) {  // averge maybe > maxthself
            if (dvalue-- > 0)
                th = averge + 1;
            else
                th = averge;
            funcMap[it.first] = new ThreadPool<In, Out>(th, it.second.second, it.second.first, queuemap[it.first]);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        printf("GetEnableSize: ");
        for (auto& it : funcMap) printf("%s[%d], ", it.first.c_str(), it.second->GetEnableSize());
        printf("\n\n");
        // printf("GetWorkingSize: ");
        // for (auto& it : funcMap) printf("%s[%d], ", it.first.c_str(), it.second->GetWorkingSize());
        // printf("\n\n");

        ch = new std::thread(&ThreadPoolManage::change, this);
    }

    ~ThreadPoolManage() {
        for (auto& it : funcMap) delete it.second;
        delete ch;
    }

  private:
    int totalThNum, changeInterval;
    std::thread* ch;
    std::unordered_map<std::string, std::pair<Engin, int>> funcThMap;
    std::unordered_map<std::string, ThreadPool<In, Out>*> funcMap;

    void change() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(changeInterval));
            ReverseThread();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            printf("GetEnableSize: ");
            for (auto& it : funcMap) printf("%s[%d], ", it.first.c_str(), it.second->GetEnableSize());
            printf("\n\n");
            // printf("GetWorkingSize: ");
            // for (auto& it : funcMap) printf("%s[%d], ", it.first.c_str(), it.second->GetWorkingSize());
            // printf("\n\n");
        }
    }

  private:
    bool cmp(std::pair<std::string, long long> x, std::pair<std::string, long long> y) { return x.second > y.second; }

    void ReverseThread() {
        // now done: kw[17250], sid[52307], lid[63185], bamp[64062],
        long long total = 0;
        printf("now done: ");
        std::unordered_map<std::string, long long> done;
        for (auto& it : funcMap) {
            done[it.first] = it.second->donenum;
            total += done[it.first];
            printf("%s[%lld], ", it.first.c_str(), done[it.first]);
        }
        printf("\n");

        // now done reverse: bamp[17250], kw[64062], lid[52307], sid[63185],
        printf("now done reverse: ");
        std::vector<std::pair<std::string, long long>> reverseDone;
        for (int n = 0; n < done.size() / 2; n++) {
            long long max = INT64_MIN, min = INT64_MAX;
            std::pair<std::string, long long> maxit, minit;
            for (auto it : done) {
                if (it.second > max && it.second != INT64_MIN) max = it.second, maxit = it;
                if (it.second < min && it.second != INT64_MIN) min = it.second, minit = it;
            }
            done[maxit.first] = INT64_MIN;
            done[minit.first] = INT64_MIN;
            reverseDone.push_back(std::make_pair(maxit.first, minit.second));
            printf("%s[%lld], ", reverseDone[reverseDone.size() - 1].first.c_str(), reverseDone[reverseDone.size() - 1].second);
            reverseDone.push_back(std::make_pair(minit.first, maxit.second));
            printf("%s[%lld], ", reverseDone[reverseDone.size() - 1].first.c_str(), reverseDone[reverseDone.size() - 1].second);
        }
        printf("\n");

        // now done reverse sorted: kw[64062], sid[63185], lid[52307], bamp[17250],
        std::sort(reverseDone.begin(), reverseDone.end(), std::bind(&ThreadPoolManage::cmp, this, std::placeholders::_1, std::placeholders::_2));  // min-->max
        printf("now done reverse sorted: ");
        for (auto& d : reverseDone) printf("%s[%lld], ", d.first.c_str(), d.second);
        printf("\n");

        // rate reverse sorted: kw[0.33], sid[0.32], lid[0.27], bamp[0.09],
        printf("rate reverse sorted: ");
        std::vector<std::pair<std::string, double>> rt;
        for (auto& it : reverseDone) {
            auto v = ((double)it.second) / total;
            rt.push_back(std::make_pair(it.first, v));
            printf("%s[%.2f], ", it.first.c_str(), v);
        }
        printf("\n");

        // thread number sorted: kw[32], sid[32], lid[26], bamp[10],
        printf("thread number sorted: ");
        std::vector<std::pair<std::string, int>> threads;
        int tmp = 0;
        for (auto it = rt.begin(); it != rt.end(); it++) {
            if (it + 1 != rt.end()) {
                int th = it->second * totalThNum;
                tmp += th;
                threads.push_back(std::make_pair(it->first, th));
            } else
                threads.push_back(std::make_pair(it->first, totalThNum - tmp));
            printf("%s[%d], ", threads[threads.size() - 1].first.c_str(), threads[threads.size() - 1].second);
        }
        printf("\n");

        // thread number sorted revised: kw[32], sid[32], lid[26], bamp[10],
        printf("thread number sorted revised: ");
        int overflow = 0;
        for (auto& it : threads) {  // 2,8,5,3 --6--> 2,6,6,4
            if (it.second > funcThMap[it.first].second) {
                overflow += (it.second - funcThMap[it.first].second);
                it.second = funcThMap[it.first].second;
            }
        }
        for (auto& it : threads) {
            if (it.second < funcThMap[it.first].second && overflow > 0) {
                int v = funcThMap[it.first].second - it.second > overflow ? overflow : funcThMap[it.first].second - it.second;
                it.second += v;
                overflow -= v;
            }
            printf("%s[%d], ", it.first.c_str(), it.second);
        }
        printf("\n");

        for (auto& it : threads) funcMap[it.first]->ChangeEnable(it.second);
    }
};

#endif
