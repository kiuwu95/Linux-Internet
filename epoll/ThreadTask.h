#pragma once
#include<stdio.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/epoll.h>
#include<fcntl.h>
#include<errno.h>
#include<string.h>
#include"SetNonblocking.h"

void ClientAccept(const int& sfd, const int& epfd);
void SendMsg(const int& cfd, const int& epfd);

void ClientAccept(const int& sfd, const int& epfd) {
    while (1) {
        struct sockaddr_in caddr;
        socklen_t caddr_len = sizeof(caddr);
        bzero(&caddr, sizeof(caddr));

        int cfd = accept(sfd, (struct sockaddr*)&caddr, &caddr_len);

        if (cfd == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return;
            }
            perror("accept");
            return;
        }

        struct epoll_event ev;
        bzero(&ev, sizeof(ev));
        if (SetNonblocking(cfd) == -1) {
            perror("SetNonblocking");
            close(cfd);
            return;
        }
        ev.data.fd = cfd;
        ev.events = EPOLLIN | EPOLLET;

        char ip[32];
        printf("Client has connected, IP: %s, 端口: %d\n",
            inet_ntop(AF_INET, &caddr.sin_addr, ip, sizeof(ip)),
            ntohs(caddr.sin_port)
        );

        if (epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev) == -1) {
            perror("epoll_ctl");
            close(cfd);
        }
    }
}

void SendMsg(const int& cfd, const int& epfd) {
    char buf[16], msg[1024];
    bzero(&msg, sizeof(msg));
    while (1) {
        int len = recv(cfd, buf, sizeof(buf), 0);

        if (len > 0) {
            strncat(msg, buf, len);
        }
        else if (len == 0) {
            printf("Client has disconnected...\n");
            epoll_ctl(epfd, EPOLL_CTL_DEL, cfd, NULL);
            close(cfd);
            break;
        }
        else {
            if (errno == EAGAIN) {
                printf("Client's port: %d\n", cfd);
                printf("Client says: %s\n", msg);
                send(cfd, msg, strlen(msg) + 1, 0);
                printf("数据接收完毕...\n");
                bzero(&msg, sizeof(msg));
            }
            else {
                perror("recv");
            }
            break;
        }
    }
}