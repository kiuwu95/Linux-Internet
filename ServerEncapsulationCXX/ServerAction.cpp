#include"Server.h"
EpollServer ES;
int main() {
    if (ES.ServerBind(9999) == -1) {
        perror("ServerBind");
        return -1;
    }
    if (ES.ServerListen() == -1) {
        perror("ServerListen");
        return -1;
    }
    if (ES.SNB() == -1) {
        perror("SNB");
        return -1;
    }
    if (ES.EpollCreate() == -1) {
        perror("EpollCreate");
        return -1;
    }
    ES.EpollWait();
}