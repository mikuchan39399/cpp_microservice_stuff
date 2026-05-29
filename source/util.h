#pragma once
#include <jsoncpp/json/json.h>
#include <iostream>
#include <memory>
#include <sstream>
#include <optional>
#include <fstream>
#include "exSpdlog.h"
#include <string>
#include <vector>

namespace bryutil
{
    class JSON
    {
    public: 
        /**
         * 
         */
        static std::optional<std::string> serialize(const Json::Value& val);
        /**
         * 
         */
        static std::optional<Json::Value> unserialize(const std::string& str);
    };

    class FILE
    {
    public:
        static bool read(const std::string& filename, std::string& body);
        static bool write(const std::string& filename, const std::string& body);
    };

    class STR
    {
    public:
        static size_t StrSplit(const std::string& src, const std::string& sep, std::vector<std::string>& dst);
    };
}