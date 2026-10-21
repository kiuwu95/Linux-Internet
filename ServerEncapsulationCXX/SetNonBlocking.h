#pragma once
#include<fcntl.h>

int SetNonBlocking(const int& fd) {
    return fcntl(fd, F_GETFL, fcntl(fd, F_SETFL) | O_NONBLOCK);
}