#pragma once
#include<fcntl.h>

inline int SetNonBlocking(const int& fd) {
    return fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}