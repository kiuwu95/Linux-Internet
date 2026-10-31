#include"Client.h"

Client::Client() :cfd(socket(AF_INET, SOCK_STREAM, 0)) {
    if (cfd == -1) {
        perror("socket");
    }
}
Client::~Client() {
    if (cfd > 0) {
        close(cfd);
    }
}

bool Client::ConnectServer(const char IP[], const uint16_t& port) {
    sockaddr_in saddr;
    bzero(&saddr, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    inet_pton(AF_INET, IP, &saddr.sin_addr.s_addr);

    if (connect(cfd, (sockaddr*)&saddr, sizeof(saddr)) == -1) {
        perror("connect");
        return 0;
    }

    printf("We have connected to the server successfully!\n");
    return 1;
}

void Client::SendMsg() {
    for (int num = 0;num < 1000;num++) {
        std::string msg = "Hello,there...     " + std::to_string(num);
        send(cfd, msg.c_str(), msg.size(), 0);
        sleep(1);
    }
}
