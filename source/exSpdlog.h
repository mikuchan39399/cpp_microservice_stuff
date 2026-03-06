#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/async.h>
#include <memory>

namespace exSpdlog
{
    struct log_settings
    {
        bool async;         // 是否启用异步日志模式
        int level;          // 日志输出等级 1-debug 2-info 3-warn 4-error 6-off
        std::string format; // 日志输出格式
        std::string path;   // 日志输出目标 stdout or file path-name
    };

    // 声明全局日志器
    extern std::shared_ptr<spdlog::logger> g_logger;
    // 声明全局日志器初始化接口
    extern void ex_log_init(const log_settings& settings);
    // 封装日志输出宏
    #define DEBUG(fmt, ...) exSpdlog::g_logger->debug("[{}:{}]: " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
    #define INFO(fmt, ...) exSpdlog::g_logger->info("[{}:{}]: " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
    #define WARN(fmt, ...) exSpdlog::g_logger->warn("[{}:{}]: " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
    #define ERROR(fmt, ...) exSpdlog::g_logger->error("[{}:{}]: " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
}