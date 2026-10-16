#pragma once
#include<fcntl.h>

int SetNonblocking(const int& fd) {
    return fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}