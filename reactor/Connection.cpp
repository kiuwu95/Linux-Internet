#include"Connection.h"
#include"Channel.h"
#include"GetTime.h"

Connection::Connection(Channel* ch) :ch_(ch) {}

std::string Connection::RecvMsg() {
    while (1) {
        uint32_t MSGSIZE;
        ssize_t len = recv(ch_->fd(), &MSGSIZE, sizeof(uint32_t), 0);
        if (len == sizeof(uint32_t)) {
            MSGSIZE = ntohl(MSGSIZE);
            len = recv(ch_->fd(), buff, MSGSIZE, 0);

            if (len < MSGSIZE) {
                break;
            }

            rmsg.clear();
            rmsg.append(buff, MSGSIZE);
            return rmsg;
        }
        else {
            if (len == 0) {
                std::cout << GetTime::GetCurrentTime();
                std::cout << "Client " << ch_->fd() << " has disconnected..." << std::endl;
                ch_->RemoveChannel();
                break;
            }
            else {
                if (errno == EAGAIN || errno == EWOULDBLOCK || len < 4) {
                    break;
                }
                if (errno == EINTR) {
                    continue;
                }
                perror("recv");
                break;
            }
        }
    }
    return "";
}

void Connection::SendMsg(std::string_view Msg) {
    char Msg_c_str[4 + Msg.size()];
    uint32_t MSGSIZE = htonl(Msg.size());

    memcpy(Msg_c_str, &MSGSIZE, 4);
    memcpy(Msg_c_str + 4, Msg.data(), Msg.size());

    if (send(ch_->fd(), Msg_c_str, Msg.size() + 4, 0) == -1) {
        perror("send");
    }
}