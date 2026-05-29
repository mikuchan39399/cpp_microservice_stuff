#pragma once
extern "C" 
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/error.h>
}
#include <iostream>
#include <vector>
#include <string>
#include "util.h"
#include "exSpdlog.h"
#include <sstream>

namespace exFFmpeg
{
    const std::string HLS_EXTM3U{"#EXTM3U"};
    const std::string HLS_VERSION{"#EXT-X-VERSION:"};
    const std::string HLS_TARGETDURATION{"#EXT-X-TARGETDURATION:"};
    const std::string HLS_SEQUENCE{"#EXT-X-MEDIA-SEQUENCE:"}; 
    const std::string HLS_PLAYLIST_TYPE{"#EXT-X-PLAYLIST-TYPE:"}; 
    const std::string HLS_INDEPENDENT_SEGMENTS{"#EXT-X-INDEPENDENT-SEGMENTS"};
    const std::string HLS_EXTINF{"#EXTINF:"}; 
    const std::string HLS_ENDLIST{"#EXT-X-ENDLIST"};
    const std::string HLS_DISCONTINUITY{"#EXT-X-DISCONTINUITY"}; // 视频流拼接/插播广告时的时间戳断层标志
    const std::string HLS_KEY{"#EXT-X-KEY:"};                    // 视频切片加密(DRM)配置标签
    class M3U8Info
    {
    public:
        struct TSSegment 
        { 
            std::string duration; 
            std::string filename; 
        };
        M3U8Info(const std::string& filename);
        bool parse();
        bool write();
        const std::vector<std::string>& headers() const;
        const std::vector<TSSegment>& tses() const;
    private:
        std::string _filename;
        std::vector<std::string> _headers;
        std::vector<TSSegment> _tses;
    };

    struct hls_settings
    {
        size_t hls_time;
        std::string playlist_type;
        std::string base_url;
    };
    class HLSTranscoder
    {
    public:
        HLSTranscoder(const hls_settings& settings);
        bool transcode(const std::string& input, const std::string& output);
    private:
        hls_settings _settings;
        std::string _last_error;
    };

    static std::string GetAVError(int errnum);
}