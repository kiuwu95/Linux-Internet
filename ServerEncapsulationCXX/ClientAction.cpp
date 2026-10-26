#include"Client.h"
#include<iostream>
Client client;
int main() {
    if (client.ConnectServer("127.0.0.1", 9999) == -1) {
        perror("ConnectServer");
        return -1;
    }

    std::string msg;
    while (1) {
        std::getline(std::cin, msg);
        {
            std::unique_lock<std::mutex> lock(client.write_mtx);
            if (client.SendMsg(msg, MsgType::MSG_) == -1) {
                perror("SendMsg");
            }
        }
    }
}
