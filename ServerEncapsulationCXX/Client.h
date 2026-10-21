#pragma once
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdio.h>
#include<string> 

class Client {
public:
    Client();
    ~Client();

    int ConnectServer(const char IP[], const unsigned short& port);

    int SendMsg(const std::string& msg);
    std::string RecvMsg();
private:
    int cfd;
private:
    int writen(const char* msg, const int& size);
    int readn(char* buf, const int& size);
};