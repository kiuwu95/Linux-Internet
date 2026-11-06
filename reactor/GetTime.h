#pragma once

#include<chrono>
#include<string>
#include<memory>
#include<ctime>

class GetTime {
public:
    GetTime& operator=(const GetTime&) = delete;
    GetTime(const GetTime&) = delete;

    static std::string GetCurrentTime() {
        auto current_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        tm* cur_time = localtime(&current_time);

        return "[" + std::to_string((cur_time)->tm_year + 1900) + "-" + std::to_string((cur_time)->tm_mon + 1) +
            "-" + std::to_string((cur_time)->tm_mday) + "    " + std::to_string((cur_time)->tm_hour) + ":" +
            std::to_string((cur_time)->tm_min) + ":" + std::to_string((cur_time)->tm_sec) + "]";
    }
};