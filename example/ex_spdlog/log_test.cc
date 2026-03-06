#include "exSpdlog.h"
#include <gflags/gflags.h>

DEFINE_bool(log_async, true, "是否启用异步日志, 默认使用");
DEFINE_int32(log_level, 1, "日志输出等级, 默认为 1: 1-debug 2-info 3-warn 4-error 6-off");
DEFINE_string(log_format, "[%H:%M:%S][%-7l]%v", "日志输出格式, 默认为[%H:%M:%S][%-7l]%v");
DEFINE_string(log_path, "stdout", "日志输出路径, 默认为标准输出流");

int main(int argc, char* argv[])
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    exSpdlog::log_settings settings{
        FLAGS_log_async, 
        FLAGS_log_level, 
        FLAGS_log_format, 
        FLAGS_log_path
    };
    exSpdlog::ex_log_init(settings);
    DEBUG("MIKU");
    INFO("Miku is {}", 16);
}