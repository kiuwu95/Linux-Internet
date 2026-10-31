#include"TcpServer.h"
#include"EventLoop.h"
#include"Channel.h"
#include"Acceptor.h"

TcpServer::TcpServer(const uint16_t& port, const size_t& loops_num) :
    main_loop(std::make_unique<EventLoop>()),
    acceptor_(std::make_unique<Acceptor>(main_loop.get(), port)),
    loops_index(0) {
    acceptor_->SetConnectionCallBack([this](int fd) {AcceptorConnectionCallBack(fd);});

    for (size_t i = 0;i < loops_num;i++) {
        std::unique_ptr<EventLoop>sub_loop = std::make_unique<EventLoop>();
        sub_loops.emplace_back(std::move(sub_loop));
    }
}
TcpServer::~TcpServer() {
    for (auto& it : threads_) {
        it.join();
    }
}

void TcpServer::start() {
    threads_.emplace_back([this] {
        main_loop->loop();
        });
    for (auto& it : sub_loops) {
        threads_.emplace_back([&it] {
            it->loop();
            });
    }
}

void TcpServer::AcceptorConnectionCallBack(int fd) {
    EventLoop* loop = sub_loops[loops_index++ % sub_loops.size()].get();

    loop->RunInLoop([loop, fd] {
        std::unique_ptr<Channel> newChannel = std::make_unique<Channel>(fd, loop);
        Channel* temp = newChannel.get();
        newChannel->SetReadCallBack([fd, loop, temp] {
            char buff[4096];
            while (1) {
                bzero(&buff, sizeof(buff));

                int len = recv(fd, buff, sizeof(buff), 0);

                if (len > 0) {
                    std::cout << "Client says: " << buff << std::endl << "    " << "fd: " << fd << std::endl;
                }
                else if (len == 0) {
                    std::cout << "Client has disconnected..." << std::endl;
                    loop->RemoveChannel(temp);
                    break;
                }
                else {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        break;
                    }
                    if (errno == EINTR) {
                        continue;
                    }
                    perror("recv");
                    break;
                }
            }
            });

        loop->AddChannel(std::move(newChannel));
        temp->EnableReading();
        });
}