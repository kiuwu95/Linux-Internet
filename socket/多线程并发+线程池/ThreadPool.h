#pragma once
#include<iostream>
#include<thread>
#include<queue>
#include<functional>
#include<chrono>
#include<mutex>
#include<condition_variable>
#include<vector>

class ThreadPool {
public:
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    ThreadPool(const int& thread_num) {
        for (int i = 0;i < thread_num;i++) {
            threads.emplace_back([this] {
                while (1) {
                    std::unique_lock<std::mutex>lock(mut);
                    cv.wait(lock, [this] {return end || !tasks.empty();});

                    if (end && tasks.empty())return;

                    // 从队列取出任务
                    std::function<void()>task = std::move(tasks.front());
                    tasks.pop();
                    lock.unlock();  // 执行任务前释放锁，避免长时间占用
                    task();        // 执行任务
                }
                });
        }
    }
    ~ThreadPool() {
        {
            std::unique_lock<std::mutex>lock(mut);
            end = true;  // 通知所有线程准备退出
        }

        cv.notify_all();
        for (auto& it : threads) {
            it.join();
        }
    }

    template<typename F, class... Args>
    void AddTask(F&& func, Args&& ...args) {
        std::function<void()>task = std::bind(std::forward<F>(func), std::forward<Args>(args)...);
        {
            std::unique_lock<std::mutex>lock(mut);
            tasks.emplace(std::move(task));
            cv.notify_one();  // 在锁内通知更安全，避免丢失唤醒
        }
    }

private:
    std::vector<std::thread>threads;          // 工作线程容器
    std::queue<std::function<void()>>tasks;   // 任务队列
    std::mutex mut;                           // 互斥锁，保护共享数据
    std::condition_variable cv;               // 条件变量，线程间通信
    bool end = false;                         // 线程池关闭标志
};