#pragma once
#include<arpa/inet.h>
#include<string.h>
#include<errno.h>
#include<string>
#include<iostream>
#include<memory>

class Channel;

class Connection {
public:
    Connection& operator=(const Connection&) = delete;
    Connection(const Connection&) = delete;

    Connection() = default;
    Connection(Channel* ch);
    ~Connection() = default;

    std::string RecvMsg();
    void SendMsg(const std::string& Msg);
private:
    Channel* ch_;//直接对Channel负责
    char buff[4096];//用户态缓冲区
    std::string rmsg;
};