#include "util.h"

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


    bool FILE::read(const std::string& filename, std::string& body)
    {
        std::ifstream ifs{};
        ifs.open(filename, std::ios::in | std::ios::binary);
        if (!ifs.is_open())
        {
            ERROR("打开读取文件失败: {}", filename);
            return false;
        }
        ifs.seekg(0, std::ios::end);
        size_t file_len = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        body.resize(file_len);
        ifs.read(&body[0], file_len);
        if (!ifs.good())
        {
            ERROR("读取文件数据失败: {}", filename);
            ifs.close();
            return false;
        }
        ifs.close();
        return true;
    }

    bool FILE::write(const std::string& filename, const std::string& body)
    {
        std::ofstream ofs{};
        ofs.open(filename, std::ios::out | std::ios::binary | std::ios::trunc);
        if (!ofs.is_open())
        {
            ERROR("打开写入文件失败: {}", filename);
            return false;
        }
        ofs.write(body.c_str(), body.size());
        if (!ofs.good())
        {
            ERROR("写入文件失败: {}", filename);
            ofs.close();
            return false;
        }
        ofs.close();
        return true;
    }

    size_t STR::StrSplit(const std::string& src, const std::string& sep, std::vector<std::string>& dst)
    {
        if (sep.empty()) return 0;
        size_t start = 0, pos = 0;
        while ((pos = src.find(sep, start)) != std::string::npos)
        {
            dst.emplace_back(src.substr(start, pos - start));
            // if (pos > start) { dst.emplace_back(src.substr(start, pos - start)); } // 跳过空字符串
            start = pos + sep.size();
        }
        dst.emplace_back(src.substr(start));
        return dst.size();
    }
}