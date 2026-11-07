#include"Buffer.h"

const std::string Buffer::rbuf() const { return buffer_; }
const char* Buffer::rc_buf() const { return buffer_.c_str(); }
const size_t Buffer::buf_size() const { return buffer_.size(); }
const uint32_t Buffer::left() const { return msgsize_ - readn_; }

void Buffer::Append(const char* msg, const size_t& len) {
    buffer_.append(msg, len);
    readn_ += len;
}

void Buffer::SetMsgsize(const uint32_t& size) {
    msgsize_ = size;
    readn_ = 0;
}

void Buffer::Clear() {
    buffer_.clear();
}
