#include "util.h"
#include "exSpdlog.h"

namespace bryutil
{
    std::optional<std::string> JSON::serialize(const Json::Value& val)
    {
        Json::StreamWriterBuilder builder;
        builder["indentation"] = "";
        std::unique_ptr<Json::StreamWriter> swp(builder.newStreamWriter());
        std::stringstream ss{};
        int ret = swp->write(val, &ss);
        if(ret != 0)
        {
            ERROR("序列化失败");
            return std::nullopt;
        }
        return ss.str();
    }

    std::optional<Json::Value> JSON::unserialize(const std::string& str)
    {
        Json::CharReaderBuilder builder;
        std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
        Json::Value val{};
        std::string errs{};
        bool ret = reader->parse(str.c_str(), str.c_str() + str.size(), &val, &errs);
        if(!ret)
        {
            ERROR("反序列化失败, 错误信息: {}", errs);
            return std::nullopt;
        }
        return val;
    }
}