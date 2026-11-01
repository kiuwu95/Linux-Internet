#include"Client.h"

static Client clt;

int main() {
    if (!clt.ConnectServer()) {
        perror("ConnectServer");
        return -1;
    }

    for (int num = 0;num < 1000;num++) {
        clt.SendMessage("Hello,there    " + std::to_string(num));
        sleep(1);
    }
}