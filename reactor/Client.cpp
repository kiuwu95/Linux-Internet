#include"Client.h"

Client::Client() :cfd_(socket(AF_INET, SOCK_STREAM, 0)) {
    if (cfd_ == -1) {
        perror("socket");
    }
}
Client::~Client() {
    if (cfd_ > 0) {
        close(cfd_);
    }
}

bool Client::ConnectServer(const uint16_t& port, const char IP[]) {
    sockaddr_in saddr;
    bzero(&saddr, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    inet_pton(AF_INET, IP, &saddr.sin_addr.s_addr);

    if (connect(cfd_, (sockaddr*)&saddr, sizeof(saddr)) == -1) {
        perror("connect");
        return 0;
    }

    std::cout << "We have connected to the server successfully!" << std::endl;
    return 1;
}

void Client::SendMessage(const std::string& Msg) {
    char Msg_c_str[4 + Msg.size()];
    uint32_t MSGSIZE = htonl(Msg.size());

    memcpy(Msg_c_str, &MSGSIZE, 4);
    memcpy(Msg_c_str + 4, Msg.c_str(), Msg.size());

    if (send(cfd_, Msg_c_str, Msg.size() + 4, 0) == -1) {
        perror("send");
    }
}

std::string Client::RecvMessage() {
    while (1) {
        uint32_t MSGSIZE;
        ssize_t len = recv(cfd_, &MSGSIZE, sizeof(uint32_t), 0);
        if (len == sizeof(uint32_t)) {
            MSGSIZE = ntohl(MSGSIZE);
            len = recv(cfd_, buff, MSGSIZE, 0);

            if (len < MSGSIZE) {
                break;
            }

            rmsg.clear();
            rmsg.append(buff, MSGSIZE);
            return rmsg;
        }
        else {
            if (len == 0) {
                std::cout << "Disconnected..." << std::endl;
                break;
            }
            else {
                if (errno == EAGAIN || errno == EWOULDBLOCK || len < 4) {
                    break;
                }
                if (errno == EINTR) {
                    continue;
                }
                perror("recv");
                break;
            }
        }
    }
    return "";
}