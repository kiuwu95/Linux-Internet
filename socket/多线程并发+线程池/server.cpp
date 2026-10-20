#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include"ThreadPool.h"

struct addrinfo {
    struct sockaddr_in addr;
    int fd;
};
ThreadPool pool(3);

void working(void* info);

int main() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        return -1;
    }


    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(9999);
    saddr.sin_addr.s_addr = INADDR_ANY;

    int ret = bind(fd, (struct sockaddr*)&saddr, sizeof(saddr));
    if (ret == -1) {
        perror("bind");
        return -1;
    }


    ret = listen(fd, 128);
    if (ret == -1) {
        perror("listen");
        return -1;
    }

    socklen_t addrlen = sizeof(struct sockaddr_in);
    while (1) {
        struct addrinfo* ptr = new addrinfo;
        ptr->fd = accept(fd, (struct sockaddr*)&ptr->addr, &addrlen);
        if (ptr->fd == -1) {
            perror("accept");
            delete ptr;
            ptr = nullptr;
            continue;
        }

        pool.AddTask(working, ptr);
    }

    close(fd);
    return 0;
}

void working(void* info) {
    struct addrinfo* addr_ptr = static_cast<struct addrinfo*>(info);

    char ip[32];
    printf("客户端IP: %s, 端口: %d\n",
        inet_ntop(AF_INET, &addr_ptr->addr.sin_addr, ip, sizeof(ip)),
        ntohs(addr_ptr->addr.sin_port));

    while (1) {
        char buff[1024];

        int len = recv(addr_ptr->fd, buff, sizeof(buff), 0);

        if (len > 0) {
            printf("client says: %s\n", buff);
            send(addr_ptr->fd, buff, len, 0);
        }
        else if (len == 0) {
            printf("客户端已经断开连接...\n");
            break;
        }
        else {
            perror("recv");
            break;
        }
    }

    close(addr_ptr->fd);
    delete addr_ptr;
    addr_ptr = nullptr;
}