#include"Client.h"
Client client;
int main() {
    if (client.ConnectServer("127.0.0.1", 9999) == -1) {
        perror("ConnectServer");
        return -1;
    }
    for (int i = 0;i < 100;i++) {
        std::string msg = "Hello,there...    " + std::to_string(i);
        if (client.SendMsg(msg) == -1) {
            perror("SendMsg");
        }
        sleep(1);
    }
}
