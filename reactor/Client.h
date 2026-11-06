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
    char buff[4096];
    std::atomic<bool> end;
    std::string rmsg;
    std::thread ListenThread;
};