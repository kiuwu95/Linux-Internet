#include"EventLoop.h"
#include"Channel.h"

EventLoop::EventLoop() :epfd_(epoll_create1(0)), end_(false) {
    if (epfd_ == -1) {
        perror("epoll_create1");
    }
}
EventLoop::~EventLoop() { end_ = true; }

void EventLoop::UpdateChannel(Channel* Ch) {
    epoll_event ev;
    ev.data.ptr = Ch;
    ev.events = Ch->events();
    if (epoll_ctl(epfd_, EPOLL_CTL_ADD, Ch->fd(), &ev) == -1) {
        perror("epoll_ctl");
    }
}
void EventLoop::AddChannel(std::unique_ptr<Channel> Ch_ptr) {
    Channels_.emplace(Ch_ptr->fd(), std::move(Ch_ptr));
}
void EventLoop::RemoveChannel(Channel* Ch) {
    if (epoll_ctl(epfd_, EPOLL_CTL_DEL, Ch->fd(), nullptr) == -1) {
        perror("epoll_ctl");
    }
    close(Ch->fd());

    Channels_.erase(Ch->fd());
}
void EventLoop::RunInLoop(std::function<void()>f) { f(); }

void EventLoop::loop() {
    epoll_event events_[1024];
    while (!end_) {
        int num = epoll_wait(epfd_, events_, 1024, -1);
        for (int i = 0;i < num;i++) {
            Channel* ch = static_cast<Channel*>(events_[i].data.ptr);
            ch->HandleEvent();
        }
    }
}