#pragma once
#include<arpa/inet.h>
#include<string.h>
#include<errno.h>
#include<string>
#include<iostream>
#include<memory>
#include<string_view>

class Channel;
class EventLoop;

class Connection {
public:
    Connection& operator=(const Connection&) = delete;
    Connection(const Connection&) = delete;

    Connection() = default;
    Connection(const int& fd, EventLoop* loop);
    ~Connection() = default;

    std::string RecvMsg();
    void SendMsg(std::string_view Msg) const;

    void DeleteConnection();
private:
    char buff[4096];
    std::string rmsg;
public:
    std::unique_ptr<Channel> channel_;
};