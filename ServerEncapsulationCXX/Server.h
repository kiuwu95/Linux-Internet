#pragma once
#include"SetNonBlocking.h"
#include<arpa/inet.h>
#include<sys/socket.h>
#include<unistd.h>
#include<sys/epoll.h>
#include<string.h>
#include<errno.h>
#include<string>

#define EVENTS_MAX_SIZE 1024

class EpollServer {
private:
    int m_fd, epfd;
    epoll_event ev, events[EVENTS_MAX_SIZE];
public:
    EpollServer(const int& sfd) :m_fd(sfd) {
        if (socket(AF_INET, SOCK_STREAM, 0) == -1) {
            perror("socket");
        }
    };
    ~EpollServer() {
        if (m_fd >= 0) {
            close(m_fd);
        }
        if (epfd >= 0) {
            close(epfd);
        }
    }

    void ServerBind(const unsigned short& s_port) {
        sockaddr_in saddr;
        bzero(&saddr, sizeof(saddr));
        saddr.sin_addr.s_addr = INADDR_ANY;
        saddr.sin_port = htons(s_port);
        saddr.sin_family = AF_INET;

        if (bind(m_fd, (sockaddr*)&saddr, sizeof(saddr)) == -1) {
            perror("bind");
            return;
        }
        if (listen(m_fd, SOMAXCONN) == -1) {
            perror("listen");
            return;
        }
        if (SetNonBlocking(m_fd) == -1) {
            perror("SetNonBlocking");
            close(m_fd);
            return;
        }
    }
    void EpollCreate() {
        epfd = epoll_create1(0);
        ev.data.fd = m_fd;
        ev.events = EPOLLIN;

        if (epoll_ctl(epfd, EPOLL_CTL_ADD, m_fd, &ev) == -1) {
            perror("epoll_ctl");
            close(epfd);
            close(m_fd);
            return;
        }

        while (1) {
            int num = epoll_wait(epfd, events, EVENTS_MAX_SIZE, -1);
            for (int i = 0;i < num;i++) {
                if (events[i].data.fd == m_fd) {
                    //ClientAccept
                    ClientAccept();
                }
                else {
                    //msg
                }
            }
        }
    }
    void ClientAccept() {
        while (1) {
            sockaddr_in caddr;
            socklen_t addr_len = sizeof(caddr);
            bzero(&caddr, sizeof(caddr));
            int cfd = accept(m_fd, (sockaddr*)&caddr, &addr_len);

            if (cfd == -1) {
                if (errno == EWOULDBLOCK || errno == EAGAIN) {
                    return;
                }
                perror("accept");
                return;
            }

            if (SetNonBlocking(cfd) == -1) {
                perror("SetNonBlocking");
                close(cfd);
                continue;
            }

            epoll_event ev;
            bzero(&ev, sizeof(ev));
            ev.events = EPOLLIN | EPOLLET;
            ev.data.fd = cfd;

            if (epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev) == -1) {
                perror("epoll_ctl");
                close(cfd);
                continue;
            }

            char ip[32];
            printf("Client has connected... IP: %s  Port: %d\n",
                inet_ntop(AF_INET, &caddr.sin_addr.s_addr, ip, sizeof(ip)),
                ntohs(caddr.sin_port));
        }
    }
};