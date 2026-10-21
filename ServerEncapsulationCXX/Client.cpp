#include"Client.h"

Client::Client() {
    cfd = socket(AF_INET, SOCK_STREAM, 0);
    if (cfd == -1) {
        perror("socket");
    }
}
Client::~Client() {
    if (cfd > 0) {
        close(cfd);
    }
}
int Client::ConnectServer(const char IP[], const unsigned short& port) {
    sockaddr_in saddr;
    bzero(&saddr, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    inet_pton(AF_INET, IP, &saddr.sin_addr.s_addr);

    if (connect(cfd, (sockaddr*)&saddr, sizeof(saddr)) == -1) {
        perror("connect");
        return -1;
    }

    return 0;
}
int Client::SendMsg(const std::string& msg) {
    char* data = new char[msg.size() + 4];
    int Msg_size = htonl(msg.size());
    memcpy(data, &Msg_size, 4);
    memcpy(data + 4, msg.data(), msg.size());

    int ret = writen(data, msg.size() + 4) == -1;
    delete[] data;
    return ret;
}
std::string Client::RecvMsg() {
    int Msg_size = 0;
    readn((char*)&Msg_size, 4);
    Msg_size = ntohl(Msg_size);

    char* data = new char[Msg_size + 1];
    if (readn(data, Msg_size) < Msg_size) {
        return std::string();
    }

    data[Msg_size] = '\0';
    std::string ret = std::string(data);
    delete[] data;
    return ret;
}

int Client::writen(const char* msg, const int& size) {
    const char* p = msg;
    int left = size, nwrite = 0;

    while (left > 0) {
        if ((nwrite = write(cfd, p, left)) > 0) {
            left -= nwrite;
            p += nwrite;
        }
        else if (nwrite == -1) {
            return -1;
        }
    }

    return size;
}
int Client::readn(char* buf, const int& size) {
    char* p = buf;
    int left = size, nread = 0;

    while (left > 0) {
        if ((nread = read(cfd, p, left)) > 0) {
            left -= nread;
            p += nread;
        }
        else if (nread == -1) {
            return -1;
        }
    }

    return size;
}