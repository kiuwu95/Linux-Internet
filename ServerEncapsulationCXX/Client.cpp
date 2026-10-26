#include"Client.h"

Client::Client() :cfd(socket(AF_INET, SOCK_STREAM, 0)), count(0), end(0) {
    if (cfd == -1) {
        perror("socket");
    }
}
Client::~Client() {
    if (cfd > 0) {
        close(cfd);
    }

    end = 1;
    MonitorServer.join();
    RecvThread.join();
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

    MonitorServerConnection();

    printf("We have connected to the server successfully!\n");

    return 0;
}
int Client::SendMsg(const std::string& msg, const MsgType& type) {
    char* data = new char[msg.size() + 8];
    int Msg_size = htonl(msg.size()), msgtype = htonl((int)type);

    memcpy(data, &Msg_size, 4);
    memcpy(data + 4, &msgtype, 4);
    memcpy(data + 8, msg.data(), msg.size());

    int ret = writen(data, msg.size() + 8);
    delete[] data;
    return ret;
}
std::string Client::RecvMsg() {
    int Msg_size = 0;
    int nread = readn((char*)&Msg_size, 4);
    if (nread < 4) {
        if (nread == 0) {
            return "EOF";
        }
        else {
            return "ERROR";
        }
    }

    int type = -1;
    readn((char*)&type, 4);
    type = ntohl(type);

    if (type == (int)MsgType::MSG_) {
        Msg_size = ntohl(Msg_size);
        char* data = new char[Msg_size + 1];
        if (readn(data, Msg_size) < Msg_size) {
            delete[] data;
            return "ERROR";
        }

        data[Msg_size] = '\0';
        std::string ret = std::string(data, Msg_size);
        delete[] data;
        return ret;
    }
    else if (type == (int)MsgType::HEART_) {
        count = 0;
        return "HEART_";
    }
    else {
        return "ERROR";
    }
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
        else if (nread == 0) {
            return 0;
        }
        else if (nread == -1) {
            return -1;
        }
    }

    return size;
}

void Client::MonitorServerConnection() {
    MonitorServer = std::thread([this] {
        while (!end) {
            count++;
            if (count > 5) {
                printf("Server has disconnected...\n");
                close(cfd);
                end = 1;
                break;
            }


            {
                std::unique_lock<std::mutex> lock(write_mtx);
                SendMsg("", MsgType::HEART_);
            }

            std::this_thread::sleep_for(std::chrono::seconds(10));
        }
        });
    RecvThread = std::thread([this] {
        while (!end) {
            std::string Msg = RecvMsg();
            if (Msg == "ERROR") {
                perror("RecvMsg");
            }
            else if (Msg == "EOF") {
                printf("Server has disconnected...\n");
                close(cfd);
                end = 1;
                break;
            }
            else if (Msg == "HEART_") {
                continue;
            }
            else {
                printf("Server says: %s\n", Msg.c_str());
            }
        }
        });
}
