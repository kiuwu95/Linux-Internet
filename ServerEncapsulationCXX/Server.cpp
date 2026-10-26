#include"Server.h"

EpollServer::EpollServer() :m_fd(socket(AF_INET, SOCK_STREAM, 0)), epfd(-1), end(0) {
    if (m_fd == -1) {
        perror("socket");
    }
};
EpollServer::~EpollServer() {
    if (m_fd > 0) {
        close(m_fd);
    }
    if (epfd > 0) {
        close(epfd);
    }

    end = 1;
    for (auto& it : MonitorClientThreads) {
        it.join();
    }
}

int EpollServer::ServerBind(const unsigned short& s_port) {
    sockaddr_in saddr;
    bzero(&saddr, sizeof(saddr));
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = htons(s_port);
    saddr.sin_family = AF_INET;

    if (bind(m_fd, (sockaddr*)&saddr, sizeof(saddr)) == -1) {
        perror("bind");
        return -1;
    }

    return 0;
}
int EpollServer::ServerListen() {
    if (listen(m_fd, SOMAXCONN) == -1) {
        perror("listen");
        return -1;
    }

    return 0;
}
int EpollServer::SNB() {
    if (SetNonBlocking(m_fd) == -1) {
        perror("SetNonBlocking");
        close(m_fd);
        return -1;
    }

    return 0;
}
int EpollServer::EpollCreate() {
    if ((epfd = epoll_create1(0)) == -1) {
        perror("epoll_create1");
        return -1;
    }
    ev.data.fd = m_fd;
    ev.events = EPOLLIN;

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, m_fd, &ev) == -1) {
        perror("epoll_ctl");
        close(epfd);
        close(m_fd);
        return -1;
    }

    return 0;
}
void EpollServer::EpollWait() {
    while (1) {
        int num = epoll_wait(epfd, events, EVENTS_MAX_SIZE, -1);
        for (int i = 0;i < num;i++) {
            if (events[i].data.fd == m_fd) {
                if (ClientAccept() == -1) {
                    perror("ClientAccept");
                }
            }
            else {
                std::string Msg = RecvMsg(events[i].data.fd);
                if (Msg == "ERROR") {
                    perror("RecvMsg");
                }
                else if (Msg == "EOF") {
                    printf("Client has disconnected...\n");

                    epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                    close(events[i].data.fd);
                    count.erase(events[i].data.fd);

                    break;
                }
                else if (Msg == "HEART_") {
                    continue;
                }
                else {
                    printf("CLient says: %s     fd: %d\n", Msg.c_str(), events[i].data.fd);
                }
            }
        }
    }
}
int EpollServer::ClientAccept() {
    while (1) {
        sockaddr_in caddr;
        socklen_t addr_len = sizeof(caddr);
        bzero(&caddr, sizeof(caddr));
        int cfd = accept(m_fd, (sockaddr*)&caddr, &addr_len);

        if (cfd == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return 0;
            }
            perror("accept");
            return -1;
        }

        if (SetNonBlocking(cfd) == -1) {
            perror("SetNonBlocking");
            close(cfd);
            continue;
        }

        epoll_event ev;
        bzero(&ev, sizeof(ev));
        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = cfd;

        if (epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev) == -1) {
            perror("epoll_ctl");
            close(cfd);
            continue;
        }

        MonitorClientConnections(cfd);

        char ip[32];
        printf("Client has connected successfully!  IP: %s  Port: %d\n",
            inet_ntop(AF_INET, &caddr.sin_addr.s_addr, ip, sizeof(ip)),
            ntohs(caddr.sin_port));
    }
}

int EpollServer::SendMsg(const int& cfd, const std::string& msg, const MsgType& type) {
    char* data = new char[msg.size() + 8];
    int Msg_size = htonl(msg.size()), msgtype = htonl((int)type);

    memcpy(data, &Msg_size, 4);
    memcpy(data + 4, &msgtype, 4);
    memcpy(data + 8, msg.data(), msg.size());

    int ret = writen(cfd, data, msg.size() + 8);
    delete[] data;
    return ret;
}
std::string EpollServer::RecvMsg(const int& cfd) {
    int Msg_size = 0;
    int nread = readn(cfd, (char*)&Msg_size, 4);
    if (nread < 4) {
        if (nread == 0) {
            return "EOF";
        }
        else {
            return "ERROR";
        }
    }

    int type = -1;
    readn(cfd, (char*)&type, 4);
    type = ntohl(type);

    if (type == (int)MsgType::MSG_) {
        Msg_size = ntohl(Msg_size);
        char* data = new char[Msg_size + 1];
        if (readn(cfd, data, Msg_size) < Msg_size) {
            delete[] data;
            return "ERROR";
        }

        data[Msg_size] = '\0';
        std::string ret = std::string(data, Msg_size);
        delete[] data;
        return ret;
    }
    else if (type == (int)MsgType::HEART_) {

        {
            std::unique_lock<std::mutex> lock(mtx);
            count[cfd] = 0;
        }

        return "HEART_";
    }
    else {
        return "ERROR";
    }
}

int EpollServer::writen(const int& cfd, const char* msg, const int& size) {
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
int EpollServer::readn(const int& cfd, char* buf, const int& size) {
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

void EpollServer::MonitorClientConnections(int cfd) {
    count.insert({ cfd,0 });
    MonitorClientThreads.emplace_back([this, cfd]() {
        while (!end) {
            {
                std::unique_lock<std::mutex> lock(mtx);
                count[cfd]++;
                if (count[cfd] > 5) {
                    printf("Client %d has disconnected...\n", cfd);

                    epoll_ctl(epfd, EPOLL_CTL_DEL, cfd, NULL);
                    close(cfd);
                    count.erase(cfd);

                    lock.unlock();
                    break;
                }
            }
            SendMsg(cfd, "", MsgType::HEART_);

            std::this_thread::sleep_for(std::chrono::seconds(10));
        }
        });
}
