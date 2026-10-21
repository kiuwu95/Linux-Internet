#pragma once
#include"SetNonBlocking.h"
#include<arpa/inet.h>
#include<sys/socket.h>
#include<unistd.h>
#include<sys/epoll.h>
#include<string.h>
#include<errno.h>
#include<stdio.h>
#include<string>

#define EVENTS_MAX_SIZE 1024

class EpollServer {
public:
    EpollServer();
    ~EpollServer();

    int ServerBind(const unsigned short& s_port);
    int ServerListen();
    int SNB();
    int EpollCreate();
    void EpollWait();
    int ClientAccept();

    int SendMsg(const int& cfd, const std::string& msg);
    std::string RecvMsg(const int& cfd);
private:
    int m_fd, epfd;
    epoll_event ev, events[EVENTS_MAX_SIZE];
private:
    int writen(const int& cfd, const char* msg, const int& size);
    int readn(const int& cfd, char* buf, const int& size);
};