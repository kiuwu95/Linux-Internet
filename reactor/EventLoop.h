#pragma once
#include<sys/epoll.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdio.h>
#include<vector>
#include<memory>
#include<atomic>
#include<functional>
#include<unordered_map>

class Channel;

class EventLoop {
public:
    EventLoop& operator=(const EventLoop&) = delete;
    EventLoop(const EventLoop&) = delete;

    EventLoop();
    ~EventLoop();

    void UpdateChannel(Channel* Ch);
    void AddChannel(std::unique_ptr<Channel> Ch_ptr);
    void RemoveChannel(Channel* Ch);
    void RunInLoop(std::function<void()>f);

    void loop();
private:
    int epfd_;
    std::atomic<bool> end_;
    std::unordered_map<int, std::unique_ptr<Channel>> Channels_;
};