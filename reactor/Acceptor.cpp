#include"Acceptor.h"
#include"EventLoop.h"
#include"Channel.h"

Acceptor::Acceptor(EventLoop* loop, const uint16_t& port) :main_loop(loop) {
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1) {
        perror("socket");
        return;
    }
    sockaddr_in saddr;
    saddr.sin_port = htons(port);
    saddr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &saddr.sin_addr.s_addr);
    if (bind(lfd, (sockaddr*)&saddr, sizeof(saddr)) == -1) {
        perror("bind");
        return;
    }
    if (listen(lfd, SOMAXCONN) == -1) {
        perror("listen");
        return;
    }

    listen_channel = std::make_unique<Channel>(lfd, loop);

    listen_channel->SetReadCallBack([this] {AcceptClient();});

    listen_channel->EnableReading();
}
void Acceptor::SetConnectionCallBack(std::function<void(int)> cb) {
    conn_cb = std::move(cb);
}
void Acceptor::AcceptClient() {
    while (1) {
        sockaddr_in caddr;
        socklen_t addr_len = sizeof(caddr);
        bzero(&caddr, sizeof(caddr));
        int cfd = accept(listen_channel->fd(), (sockaddr*)&caddr, &addr_len);
        if (cfd == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            }
            perror("accept");
            break;
        }

        conn_cb(cfd);
    }
}