#pragma once
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdio.h>
#include<string> 
#include<iostream>
#include<string_view>
#include<thread>
#include<atomic>
#include<memory>

class Buffer;

class Client {
public:
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

    Client();
    ~Client();

    bool ConnectServer(const uint16_t& port = 9999, const char IP[] = "127.0.0.1");
    void StartListen();

    void GetMessage();
    void SendMessage(std::string_view Msg) const;
    std::string RecvMessage();
private:
    int cfd_;
    std::atomic<bool> end;
    std::thread ListenThread;

    std::unique_ptr<Buffer> read_buff_;
    std::unique_ptr<Buffer> write_buff_;
};