#pragma once
#include"SetNonBlocking.h"
#include"MsgType.h"
#include<arpa/inet.h>
#include<sys/socket.h>
#include<unistd.h>
#include<sys/epoll.h>
#include<string.h>
#include<errno.h>
#include<stdio.h>
#include<string>
#include<thread>
#include<vector>
#include<mutex>
#include<chrono>
#include<atomic>
#include<unordered_map>

#define EVENTS_MAX_SIZE 1024

class EpollServer {
public:
    EpollServer(const EpollServer& ES) = delete;
    EpollServer& operator=(const EpollServer& ES) = delete;

    EpollServer();
    ~EpollServer();

    int ServerBind(const unsigned short& s_port);
    int ServerListen();
    int SNB();
    int EpollCreate();
    void EpollWait();
    int ClientAccept();

    int SendMsg(const int& cfd, const std::string& msg, const MsgType& type);
    std::string RecvMsg(const int& cfd);
private:
    int m_fd, epfd;
    epoll_event ev, events[EVENTS_MAX_SIZE];

    std::unordered_map<int, int>count;
    std::vector<std::thread>MonitorClientThreads;
    std::atomic<bool> end;
    std::mutex mtx;
private:
    int writen(const int& cfd, const char* msg, const int& size);
    int readn(const int& cfd, char* buf, const int& size);
    void MonitorClientConnections(int cfd);
};