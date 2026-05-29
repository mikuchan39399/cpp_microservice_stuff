#include "../../source/exFFmpeg.h"


void hsl_test()
{
    exFFmpeg::hls_settings settings{
        .hls_time = 10, 
        .playlist_type = "vod", 
        .base_url = "http://192.168.239.128:9000/video/"
    };
    exFFmpeg::HLSTranscoder transcoder{settings};
    bool ret = transcoder.transcode("./test.mp4", "./dest.m3u8");
    if (!ret)
    {
        ERROR("转封装失败");
        abort();
    }

}

void m3u8_test()
{
    exFFmpeg::M3U8Info info{"./dest.m3u8"};
    bool ret = info.parse();
    if (!ret)
    {
        ERROR("m3u8文件解析失败");
        return;
    }
    const auto& headers = info.headers();
    for (auto& h : headers)
    {
        std::cout << "[" << h << "]" << std::endl;
    }
    const auto& tses = info.tses();
    for (const auto& ts : tses)
    {
        std::cout << "[" << ts.duration << "]" << std::endl;
        std::cout << "[" << ts.filename << "]" << std::endl;
    }
    ret = info.write();
    if (!ret)
    {
        ERROR("m3u8文件重写失败");
        abort();
    }
}

int main()
{
    hsl_test();
    m3u8_test();
    return 0;
}