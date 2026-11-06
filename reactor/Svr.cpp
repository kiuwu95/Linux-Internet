#include"TcpServer.h"
#include<string>
#include<iostream>

static TcpServer Reactor(15);

int main() {
    Reactor.start();

    std::string quit_;
    while (1) {
        std::cin >> quit_;
        if (quit_ == "quit") {
            break;
        }
    }
}