#include<stdio.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/epoll.h>
#include"ThreadPool.h"
#include"ThreadTask.h"

#define MAX_SIZE 1024

ThreadPool pool(MAX_SIZE);

int main() {
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in saddr;
    bzero(&saddr, sizeof(saddr));
    saddr.sin_port = htons(9999);
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sfd, (struct sockaddr*)&saddr, sizeof(saddr)) == -1) {
        perror("bind");
        return -1;
    }
    if (listen(sfd, 128) == -1) {
        perror("listen");
        return -1;
    }
    if (SetNonblocking(sfd) == -1) {
        perror("SetNonblocking");
        close(sfd);
        return -1;
    }
    int epfd = epoll_create1(0);
    if (epfd == -1) {
        perror("epoll_create1");
        return -1;
    }

    struct epoll_event ev, events[MAX_SIZE];
    bzero(&ev, sizeof(ev));
    bzero(&events, sizeof(events));
    ev.data.fd = sfd;
    ev.events = EPOLLIN;

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, sfd, &ev) == -1) {
        perror("epoll_ctl");
        close(epfd);
        close(sfd);
        return -1;
    }

    while (1) {
        int num = epoll_wait(epfd, events, MAX_SIZE, -1);
        for (int i = 0;i < num;i++) {
            if (events[i].data.fd == sfd) {
                ClientAccept(sfd, epfd);
            }
            else {
                int cfd = events[i].data.fd;
                pool.AddTask(SendMsg, cfd, epfd);
            }
        }
    }

    close(epfd);
    close(sfd);
    return 0;
}