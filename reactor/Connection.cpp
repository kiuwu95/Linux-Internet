#include"Connection.h"
#include"Channel.h"
#include"GetTime.h"
#include"EventLoop.h"

Connection::Connection(const int& fd, EventLoop* loop) :
    read_buff_(std::make_unique<Buffer>()),
    write_buff_(std::make_unique<Buffer>()),
    channel_(std::make_unique<Channel>(fd, loop)) {
}
const std::string Connection::RecvMsg() {
    int len = 0;
    char buf[1024];
    while (1) {
        if (read_buff_->left() == 0) {
            read_buff_->Clear();

            uint32_t MSGSIZE;
            len = recv(channel_->fd(), &MSGSIZE, 4, 0);
            if (len == 4) {
                MSGSIZE = ntohl(MSGSIZE);
                read_buff_->SetMsgsize(MSGSIZE);
            }
            else {
                if (len == 0) {
                    std::cout << GetTime::GetCurrentTime();
                    std::cout << "Client " << channel_->fd() << " has disconnected..." << std::endl;
                    this->DeleteConnection();
                    break;
                }
                else {
                    if (len < 4) {
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

        len = recv(channel_->fd(), buf, read_buff_->left(), 0);
        if (len == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            }
            if (errno == EINTR) {
                continue;
            }
            perror("recv");
            break;
        }
        read_buff_->Append(buf, len);

        if (read_buff_->left() == 0) {
            return read_buff_->rbuf();
        }
    }

    return "";
}

void Connection::SendMsg(std::string_view Msg) const {
    uint32_t MSGSIZE = htonl(Msg.size());
    write_buff_->Append((char*)&MSGSIZE, 4);
    write_buff_->Append(Msg.data(), Msg.size());

    if (send(channel_->fd(), write_buff_->rc_buf(), Msg.size() + 4, 0) == -1) {
        perror("send");
    }

    write_buff_->Clear();
}

void Connection::DeleteConnection() {
    channel_->loop()->DeleteConnection(channel_->fd());
}

Channel* Connection::channel() const {
    return channel_.get();
}
