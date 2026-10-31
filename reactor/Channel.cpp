#include"Channel.h"
#include"EventLoop.h"

Channel::Channel(const int& fd, EventLoop* loop) :
    fd_(fd),
    loop_(loop) {
    bzero(&event, sizeof(event));
};
void Channel::RemoveChannel() {
    loop_->RemoveChannel(this);
}

int Channel::fd() const { return fd_; }
uint32_t Channel::events() const { return event; }

void Channel::UpdateRevent(const uint32_t& UR) { event = UR; }
void Channel::EnableReading() {
    event |= EPOLLIN;
    loop_->UpdateChannel(this);
}
void Channel::HandleEvent() {
    if (event & EPOLLIN) {
        readcall_back();
    }
    else if (event & EPOLLOUT) {
        writecall_back();
    }
}

void Channel::SetReadCallBack(std::function<void()> f) {
    readcall_back = std::move(f);
}
void Channel::SetWriteCallBack(std::function<void()> f) {
    writecall_back = std::move(f);
}