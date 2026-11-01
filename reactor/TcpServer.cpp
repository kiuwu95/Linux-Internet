#include"TcpServer.h"
#include"EventLoop.h"
#include"Channel.h"
#include"Acceptor.h"
#include"Connection.h"

TcpServer::TcpServer(const size_t& loops_num, const uint16_t& port, const char IP[]) :
    main_loop(std::make_unique<EventLoop>()),
    acceptor_(std::make_unique<Acceptor>(main_loop.get(), port, IP)),
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

    loop->RunInLoop([fd, loop] {
        std::unique_ptr<Channel> newChannel = std::make_unique<Channel>(fd, loop);
        Channel* temp = newChannel.get();
        newChannel->SetReadCallBack([temp, fd] {
            std::string msg = temp->loop()->Connections_[fd]->RecvMsg();
            if (msg != "")
                std::cout << msg << "    " << "fd: " << fd << std::endl;
            });

        loop->AddChannel(std::move(newChannel));
        temp->EnableReading();
        });
}