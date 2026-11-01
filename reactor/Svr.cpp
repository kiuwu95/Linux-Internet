#include"TcpServer.h"

static TcpServer Reactor(10);

int main() {
    Reactor.start();
}