#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>

int main() {
    // socket()：向内核申请创建一个TCP套接字
    // 返回值：成功返回【文件描述符fd，正整数】；失败返回 -1
    // fd：文件描述符，内核资源编号，代表监听socket，只用来做bind、listen、accept，**不能收发数据**
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");  // 打印系统错误原因
        return -1;
    }

    // sockaddr_in：IPv4专用地址结构体，存储IP地址、端口、协议族
    struct sockaddr_in saddr;//saddr表示server address，即服务端的地址信息
    memset(&saddr, 0, sizeof(saddr));   // 全部置0，避免垃圾值，sin_zero占位字段清零

    saddr.sin_family = AF_INET;         // 指定协议族：AF_INET 代表IPv4
    saddr.sin_port = htons(9999);       // 端口！！重点：必须 htons()，主机字节序转网络大端字节序
    // 千万不要直接写 saddr.sin_port = 9999，会端口错乱
    saddr.sin_addr.s_addr = INADDR_ANY; // INADDR_ANY = 0.0.0.0，监听本机所有网卡

    // bind：把socket(fd) 和 本机IP+端口绑定在一起
    // bind原型 int bind(int, const struct sockaddr*, socklen_t)
    // 函数参数需要通用结构体sockaddr*，我们定义的是sockaddr_in，所以强制类型转换
    // ret 接收系统调用返回值：成功返回0，失败返回-1；ret只是普通局部临时变量，用于错误判断
    int ret = bind(fd, (struct sockaddr*)&saddr, sizeof(saddr));
    if (ret == -1) {
        perror("bind");
        return -1;
    }

    // listen：将socket切换为监听状态，第二个参数是半连接队列最大长度
    // 复用ret变量，覆盖上一次bind的返回结果
    ret = listen(fd, 128);
    if (ret == -1) {
        perror("listen");
        return -1;
    }
    //caddr表示client address，即客户端的地址信息
    struct sockaddr_in caddr;   // accept输出参数：保存【客户端】的地址信息（客户端IP、端口）
    socklen_t addrlen = sizeof(caddr);
    // ⚠注意：accept第三个参数必须传变量地址，不能直接写sizeof(caddr)常量！

    // accept：阻塞等待客户端连接到来
    // 参数fd：监听socket；不要拿cfd去accept
    // 返回值cfd：**全新通信文件描述符**！专门用来 recv / send 和客户端通信
    // fd继续留在原地负责接收下一个连接，cfd才是读写的句柄
    int cfd = accept(fd, (struct sockaddr*)&caddr, &addrlen);
    if (cfd == -1) {
        perror("accept");
        return -1;
    }

    char ip[32];
    // inet_ntop：把结构体中整数形式的IP，转换成人类可读字符串 "127.0.0.1"
    // ntohs(caddr.sin_port)：网络字节序端口转回主机字节序，用于打印输出
    printf("客户端IP: %s, 端口: %d\n",
        inet_ntop(AF_INET, &caddr.sin_addr, ip, sizeof(ip)),
        ntohs(caddr.sin_port));

    while (1) {
        char buff[1024];
        // recv：从cfd读取客户端数据
        // 返回值 >0：读到的字节数；=0：客户端正常断开；<0：出错
        int len = recv(cfd, buff, sizeof(buff), 0);

        if (len > 0) {
            // ⚠重大坑！recv不会自动追加 '\0'！buff里面没有字符串结束符
            // 如果直接printf("%s",buff)，会越界读取内存，产生乱码风险！
            // 修复：buff[len] = '\0';
            printf("client say: %s\n", buff);
            send(cfd, buff, len, 0); // send把读到的数据原样发回客户端（回显服务器）
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

    // close 告诉内核释放文件描述符资源；不close会发生fd泄漏
    close(fd);   // 关闭监听套接字
    close(cfd);  // 关闭和客户端通信套接字
    return 0;
}
