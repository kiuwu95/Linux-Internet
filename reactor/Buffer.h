#pragma once
#include<string>
#include<cstdint>

class Buffer {
public:
    Buffer() = default;
    ~Buffer() = default;

    const std::string rbuf() const;
    const char* rc_buf() const;
    const size_t buf_size() const;
    const uint32_t left() const;

    void Append(const char* msg, const size_t& len);

    void SetMsgsize(const uint32_t& size);
    void Clear();
private:
    std::string buffer_;
    uint32_t readn_ = 0;
    uint32_t msgsize_ = 0;
};