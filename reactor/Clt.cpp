#include"Client.h"

static Client cli;

int main() {
    if (!cli.ConnectServer("127.0.0.1", 9999)) {
        perror("ConnectServer");
        return -1;
    }
    cli.SendMsg();
}