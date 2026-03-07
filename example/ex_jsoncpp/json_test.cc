#include "../../source/util.h"
#include "../../source/exSpdlog.h"
#include <iostream>

void serialize_test()
{
    Json::Value val;
    val["name"] = "Hatsune Miku";
    val["age"] = 16;
    val["cloth"].append("Sleeveless silver-grey top");
    val["cloth"].append("Signature teal necktie");
    val["cloth"].append("Black pleated skirt");
    val["cloth"].append("Black arm sleeves and thigh-high socks");
    
    auto str = bryutil::JSON::serialize(val);
    if(!str.has_value()) return;
    std::cout << "序列化输出: \n" << str.value() << std::endl;
}

void unserialize_test()
{
    std::string str = R"({"age":16,"cloth":["Sleeveless silver-grey top","Signature teal necktie","Black pleated skirt","Black arm sleeves and thigh-high socks"],"name":"Hatsune Miku"})";

    auto result = bryutil::JSON::unserialize(str);
    if(!result.has_value())
    {
        std::cout << "反序列化失败了！" << std::endl;
        return;
    }
    Json::Value val = result.value();
    
    std::cout << "反序列化读取: " << std::endl;
    std::cout << "姓名: " << val["name"].asString() << std::endl;
    std::cout << "年龄: " << val["age"].asInt() << std::endl;
    
    std::cout << "服装穿搭解析:" << std::endl;

    for (int i = 0; i < val["cloth"].size(); ++i)
    {
        std::cout << "  * " << val["cloth"][i].asString() << std::endl;
    }
}

int main()
{
    exSpdlog::log_settings settings{
        true, 
        spdlog::level::err, 
        "[%H:%M:%S][%-7l]%v", 
        "stdout"
    };
    exSpdlog::ex_log_init(settings);
    serialize_test();
    std::cout << "----------------------------------------" << std::endl;
    unserialize_test();
}
