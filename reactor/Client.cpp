#include"Client.h"
#include"GetTime.h"
#include"Buffer.h"

Client::Client() :cfd_(socket(AF_INET, SOCK_STREAM, 0)),
read_buff_(std::make_unique<Buffer>()),
write_buff_(std::make_unique<Buffer>()) {
    if (cfd_ == -1) {
        perror("socket");
    }
}
Client::~Client() {
    end = true;
    ListenThread.join();

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

    end = false;
    this->StartListen();

    std::cout << GetTime::GetCurrentTime();
    std::cout << "Connected to the server successfully!" << std::endl;

    return 1;
}

void Client::StartListen() {
    ListenThread = std::thread([this] {
        while (!end) {
            if (this->RecvMessage() != "") {
                std::cout << GetTime::GetCurrentTime() << read_buff_->rbuf() << std::endl;
            }
        }
        });
}

void Client::GetMessage() {
    std::string msg;
    while (1) {
        std::getline(std::cin, msg);
        this->SendMessage(msg);
    }
}

void Client::SendMessage(std::string_view Msg) const {
    uint32_t MSGSIZE = htonl(Msg.size());
    write_buff_->Append((char*)&MSGSIZE, 4);
    write_buff_->Append(Msg.data(), Msg.size());

    if (send(cfd_, write_buff_->rc_buf(), Msg.size() + 4, 0) == -1) {
        perror("send");
    }

    write_buff_->Clear();
}

std::string Client::RecvMessage() {
    int len = 0;
    char buf[1024];
    while (1) {
        if (read_buff_->left() == 0) {
            read_buff_->Clear();

            uint32_t MSGSIZE;
            len = recv(cfd_, &MSGSIZE, 4, 0);
            if (len == 4) {
                MSGSIZE = ntohl(MSGSIZE);
                read_buff_->SetMsgsize(MSGSIZE);
            }
            else {
                if (len == 0) {
                    std::cout << GetTime::GetCurrentTime();
                    std::cout << "Server has disconnected..." << std::endl;
                    break;
                }
                else {
                    if (len < 4) {
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

        len = recv(cfd_, buf, read_buff_->left(), 0);
        if (len == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            }
            if (errno == EINTR) {
                continue;
            }
            perror("recv");
            break;
        }
        read_buff_->Append(buf, len);

        if (read_buff_->left() == 0) {
            return read_buff_->rbuf();
        }
    }

    return "";
}