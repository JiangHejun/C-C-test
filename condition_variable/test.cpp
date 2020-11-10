#include <stdio.h>

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

std::mutex mx;               //全局互斥锁
std::condition_variable cv;  //全局条件变量
// bool ready = false;          //全局标志位

void C() {
    std::unique_lock<std::mutex> lock(mx);
    std::cout << "C begin\n" << std::endl;
    std::cout << "C wait\n" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    // if (!ready)  //如果标志位不为 true, 则等待...
    // {
    cv.wait(lock);  //前线程被阻塞, 当全局标志位变为 true 之后,线程被唤醒, 继续往下执行]
    // std::cout << "C wait...\n" << std::endl;
    // }
    std::cout << "C wakeup\n" << std::endl;
}

void B() {
    std::unique_lock<std::mutex> lock(mx);
    std::cout << "B begin\n" << std::endl;
    std::cout << "B wait\n" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    // if (!ready)  //如果标志位不为 true, 则等待...
    // {
    cv.wait(lock);  //前线程被阻塞, 当全局标志位变为 true 之后,线程被唤醒, 继续往下执行
    // std::cout << "B wait...\n" << std::endl;
    // }
    std::cout << "B wakeup\n" << std::endl;
}

void A() {
    std::unique_lock<std::mutex> lock(mx);
    std::cout << "A begin\n" << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    std::cout << "A notify one\n" << std::endl;
    // ready = true;  // 设置全局标志位为 true
    // cv.notify_all();
    cv.notify_one();  // 唤醒上面的线程

    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    std::cout << "A notify one\n" << std::endl;
    cv.notify_one();  // 唤醒上面的线程
}

int main() {
    std::thread ThreadA(A);
    std::thread ThreadC(C);
    std::thread ThreadB(B);
    ThreadC.join();
    ThreadB.join();
    ThreadA.join();
    std::cout << "over" << std::endl;

    return 0;
}