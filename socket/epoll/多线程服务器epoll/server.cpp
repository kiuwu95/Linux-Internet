#include<stdio.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/epoll.h>
#include<fcntl.h>
#include<errno.h>
#include<string.h>
#include<thread>

#define MAX_SIZE 1024

void SetNonblocking(const int& fd);
void ClientAccept(const int& sfd, const int& epfd);
void SendMsg(const int& cfd, const int& epfd);

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

    epoll_ctl(epfd, EPOLL_CTL_ADD, sfd, &ev);

    while (1) {
        int num = epoll_wait(epfd, events, MAX_SIZE, -1);
        for (int i = 0;i < num;i++) {
            if (events[i].data.fd == sfd) {
                std::thread t(ClientAccept, sfd, epfd);
                t.detach();
            }
            else {
                int cfd = events[i].data.fd;
                std::thread t(SendMsg, cfd, epfd);
                t.detach();
            }
        }
    }

    close(epfd);
    close(sfd);
    return 0;
}

void SetNonblocking(const int& fd) {
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

void ClientAccept(const int& sfd, const int& epfd) {
    struct sockaddr_in caddr;
    socklen_t caddr_len = sizeof(caddr);
    bzero(&caddr, sizeof(caddr));

    int cfd = accept(sfd, (struct sockaddr*)&caddr, &caddr_len);
    if (cfd == -1) {
        perror("accept");
        return;
    }

    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    SetNonblocking(cfd);
    ev.data.fd = cfd;
    ev.events = EPOLLIN | EPOLLET;

    char ip[32];
    printf("Client has connected, IP: %s, 端口: %d\n",
        inet_ntop(AF_INET, &caddr.sin_addr, ip, sizeof(ip)),
        ntohs(caddr.sin_port)
    );

    epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
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
