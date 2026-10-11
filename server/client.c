#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>

int main() {
    // 创建客户端socket文件描述符
    // AF_INET:ipv4，SOCK_STREAM:TCP，0协议默认
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    // fd < 0 创建套接字失败
    if (fd == -1) {
        perror("socket");
        return -1;
    }

    // sockaddr_in：ipv4地址结构体，保存服务器的ip+端口
    struct sockaddr_in saddr;
    // 设置服务器端口，htons：主机字节序转网络大端字节序
    saddr.sin_port = htons(9999);
    saddr.sin_family = AF_INET;
    // inet_pton：字符串ip "127.0.0.1" → 转为网络二进制格式，填入结构体
    inet_pton(AF_INET, "127.0.0.1", &saddr.sin_addr.s_addr);

    // connect：主动向服务器发起TCP三次握手
    // fd：客户端唯一套接字；连接成功后这个fd直接用来收发数据
    // 客户端没有监听fd！客户端是主动发起连接方
    int ret = connect(fd, (struct sockaddr*)&saddr, sizeof(saddr));
    if (ret == -1) {
        perror("connect");
        close(fd); // 出错记得关闭文件描述符，防止资源泄漏
        return -1;
    }

    int num = 0;
    while (1) {
        char buff[1024];
        sprintf(buff, "hello, there, %d\n", num++);
        send(fd, buff, strlen(buff) + 1, 0); // strlen取字符串有效字符长度

        memset(buff, 0, sizeof(buff));
        // recv：从服务器读取数据
        int len = recv(fd, buff, sizeof(buff), 0);
        if (len > 0) {
            // len是实际读到的字节数量
            printf("server says: %s\n", buff);
        }
        else if (len == 0) {
            // recv返回0：对端调用close，TCP连接正常断开
            printf("服务器端已经断开连接...\n");
            break;
        }
        else {
            // len=-1，读取出错
            perror("recv");
            break;
        }
        sleep(1);
    }

    // 关闭客户端套接字文件描述符，释放内核资源
    close(fd);
    return 0;
}
