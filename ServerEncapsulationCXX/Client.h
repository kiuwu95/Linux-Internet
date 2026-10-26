#pragma once
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdio.h>
#include<string> 
#include<mutex>
#include<thread>
#include<chrono>
#include<atomic>
#include"MsgType.h"

class Client {
public:
    std::mutex write_mtx;
public:
    Client(const Client& clt) = delete;
    Client& operator=(const Client& clt) = delete;

    Client();
    ~Client();

    int ConnectServer(const char IP[], const unsigned short& port);

    int SendMsg(const std::string& msg, const MsgType& type);
    std::string RecvMsg();
private:
    int cfd;
    std::atomic<int>count;
    std::atomic<bool>end;
    std::thread MonitorServer, RecvThread;
private:
    int writen(const char* msg, const int& size);
    int readn(char* buf, const int& size);

    void MonitorServerConnection();
};