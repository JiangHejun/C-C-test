/*
 * @Description: BlockingQueue.h use to save queue
 * @Author: Hejun Jiang
 * @Date: 2020-11-06 10:19:54
 * @LastEditTime: 2020-11-10 17:17:47
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

template <typename T, typename Container = std::queue<T>>
class BlockingQueue {
  public:
    using container_type = Container;
    using value_type = typename Container::value_type;
    using reference = typename Container::reference;
    using const_reference = typename Container::const_reference;
    using size_type = typename Container::size_type;
    using mutex_type = std::mutex;
    using condition_variable_type = std::condition_variable;

  private:
    Container queue_;
    mutable mutex_type mutex_;
    condition_variable_type cond_;

  public:
    BlockingQueue() = default;
    BlockingQueue(const BlockingQueue &) = delete;
    BlockingQueue &operator=(const BlockingQueue &) = delete;
    void pop(reference elem) {
        std::unique_lock<mutex_type> lock(mutex_);
        cond_.wait(lock, [this]() { return !queue_.empty(); });
        elem = std::move(queue_.front());
        queue_.pop();
    }
    value_type pop() {
        std::unique_lock<mutex_type> lock(mutex_);
        cond_.wait(lock, [this]() { return !queue_.empty(); });
        value_type elem = std::move(queue_.front());
        queue_.pop();
        return std::move(elem);
    }
    bool try_pop(reference elem) {
        std::unique_lock<mutex_type> lock(mutex_);
        if (queue_.empty()) {
            return false;
        }
        elem = std::move(queue_.front());
        queue_.pop();
        return true;
    }

    bool empty() const {
        std::lock_guard<mutex_type> lock(mutex_);
        return queue_.empty();
    }

    size_type size() const {
        std::lock_guard<mutex_type> lock(mutex_);
        return queue_.size();
    }

    void push(const value_type &elem) {
        {
            std::lock_guard<mutex_type> lock(mutex_);
            queue_.push(elem);
        }
        cond_.notify_one();
    }
    void push(value_type &&elem) {
        {
            std::lock_guard<mutex_type> lock(mutex_);
            queue_.push(std::move(elem));
        }
        cond_.notify_one();
    }
};

#endif