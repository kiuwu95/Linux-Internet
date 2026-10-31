#pragma once
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdio.h>
#include<string> 

class Client {
public:
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

    Client();
    ~Client();

    bool ConnectServer(const char IP[], const uint16_t& port);
    void SendMsg();
private:
    int cfd;
};