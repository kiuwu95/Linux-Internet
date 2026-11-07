#pragma once
#include<arpa/inet.h>
#include<string.h>
#include<errno.h>
#include<string>
#include<iostream>
#include<memory>

#include"Buffer.h"

class Channel;
class EventLoop;

class Connection {
public:
    Connection& operator=(const Connection&) = delete;
    Connection(const Connection&) = delete;

    Connection() = delete;
    Connection(const int& fd, EventLoop* loop);
    ~Connection() = default;

    // std::string RecvMsg();
    void SendMsg(std::string_view Msg) const;
    const std::string RecvMsg();
    void DeleteConnection();
private:
    std::unique_ptr<Buffer>read_buff_;
    std::unique_ptr<Buffer>write_buff_;

    // std::string rmsg;
public:
    std::unique_ptr<Channel> channel_;
};