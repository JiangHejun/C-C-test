/*
 * @Description: BlockingQueue.h use to save queue
 * @Author: Hejun Jiang
 * @Date: 2020-11-06 10:19:54
 * @LastEditTime: 2020-11-06 15:16:58
 * @LastEditors: Hejun Jiang
 * @Version: v0.0.1
 * @Contact: jianghejun@hccl.ioa.ac.cn
 * @Corporation: hccl
 */
#ifndef __BLOCKING_QUEUE_H__
#define __BLOCKING_QUEUE_H__
#include <condition_variable>
#include <mutex>
#include <queue>

template <typename JobType>
class BlockingQueue {
  public:
    void push(JobType job) {
        std::unique_lock<std::mutex> lock(mutex_);
        queue_.push(job);
        cond_.notify_one();
    }

    JobType pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        if (queue_.empty()) {
            printf("waiting\n");
            cond_.wait(lock);
        }
        JobType job = queue_.front();
        queue_.pop();
        return job;
    }

    size_t size() {
        std::unique_lock<std::mutex> lock(mutex_);
        return queue_.size();
    }

    bool empty() {
        std::unique_lock<std::mutex> lock(mutex_);
        return queue_.empty();
    }

  protected:
    std::mutex mutex_;
    std::condition_variable cond_;
    std::queue<JobType> queue_;
};

#endif /* __BLOCKING_QUEUE_H__ */