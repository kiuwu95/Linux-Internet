#pragma once
#include<thread>
#include<iostream>
#include<vector>

class Acceptor;
class Channel;
class EventLoop;

class TcpServer {
public:
    TcpServer(const TcpServer&) = delete;
    TcpServer& operator=(const TcpServer&) = delete;

    TcpServer(const uint16_t& port, const size_t& loops_num);
    ~TcpServer();

    void start();
private:
    void AcceptorConnectionCallBack(int fd);

private:
    std::unique_ptr<EventLoop>main_loop;
    std::unique_ptr<Acceptor>acceptor_;
    std::vector<std::unique_ptr<EventLoop>>sub_loops;
    std::vector<std::thread>threads_;
    size_t loops_index;
};