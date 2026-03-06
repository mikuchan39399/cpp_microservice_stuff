#include "exSpdlog.h"

namespace exSpdlog
{
    std::shared_ptr<spdlog::logger> g_logger;
    
    void ex_log_init(const log_settings& settings)
    {
        // - 判断日志器类型 同步或异步
        if(settings.async == true)
        {
            if(settings.path == "stdout") // 判断输出目标为 file or stdout
            {
                g_logger = spdlog::stdout_color_mt<spdlog::async_factory>("stdout_logger");
            }
            else
            {
                g_logger = spdlog::basic_logger_mt<spdlog::async_factory>("file_logger", settings.path);
            }
        }
        else
        {
            if(settings.path == "stdout")
            {
                g_logger = spdlog::stdout_color_mt("stdout_logger");
            }
            else
            {
                g_logger = spdlog::basic_logger_mt("file_logger", settings.path);
            }
        }
        // 设置日志等级
        g_logger->set_level(spdlog::level::level_enum(settings.level));
        //设置日志格式
        g_logger->set_pattern(settings.format);
    }
}