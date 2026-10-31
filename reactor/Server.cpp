#include"TcpServer.h"

static TcpServer Reactor(9999, 6);

int main() {
    Reactor.start();
}