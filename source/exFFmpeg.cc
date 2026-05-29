#include "exFFmpeg.h"

namespace exFFmpeg
{

    M3U8Info::M3U8Info(const std::string& filename)
    : _filename(filename)
    {}
    bool M3U8Info::parse()
    {
        std::string body;
        if (!bryutil::FILE::read(_filename, body)) 
        {
            return false;
        }

        std::istringstream iss{body};
        std::string line{};
        bool is_waiting_for_ts{false};
        TSSegment cur_segment{};

        while (std::getline(iss, line))
        {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            if (line.empty()) continue; // 跳过空行

            if (is_waiting_for_ts && line[0] != '#') 
            {
                cur_segment.filename = line;
                _tses.push_back(cur_segment);
                is_waiting_for_ts = false;
                continue;
            }

            if (line.find(HLS_EXTINF) == 0) 
            {
                cur_segment.duration = line.substr(8, line.size() - 9); 
                is_waiting_for_ts = true;
            }
            else if (line.find("#EXT") == 0) 
            {
                if (line.find(HLS_ENDLIST) != std::string::npos) continue;
                _headers.push_back(line);
            }
        }

        return true;
    }

    bool M3U8Info::write()
    {
        std::ostringstream oss{};
        for (const auto& header : _headers)
        {
            oss << header << "\n";
        }
        for (const auto& ts : _tses)
        {
            oss << HLS_EXTINF << ts.duration << ",\n";
            oss << ts.filename << "\n";
        }
        bool has_endlist = false;
        for (const auto& h : _headers) 
        {
            if(h.find(HLS_ENDLIST) != std::string::npos) has_endlist = true;
        }
        oss << HLS_ENDLIST << "\n";
        return bryutil::FILE::write(_filename, oss.str());
    }

    const std::vector<std::string>& M3U8Info::headers() const
    {
        return _headers;
    }

    const std::vector<M3U8Info::TSSegment>& M3U8Info::tses() const
    {
        return _tses;
    }


    HLSTranscoder::HLSTranscoder(const hls_settings& settings)
    : _settings(settings)
    , _last_error(std::string{})
    {}

    bool HLSTranscoder::transcode(const std::string& input, const std::string& output)
    {
        int ret{0};
        AVFormatContext* input_context{nullptr};
        AVFormatContext* output_context{nullptr};
        AVDictionary* dict{nullptr};
        AVPacket pkt{}; 

        // 1. 打开输入文件, 创建输入格式上下文对象
        ret = avformat_open_input(&input_context, input.c_str(), nullptr, nullptr);
        if (ret < 0)
        {
            _last_error = "打开输入文件失败: " + GetAVError(ret);
            ERROR("{}", _last_error);
            ret = -1;
            goto cleanup;
        }

        // 2. 通过输入格式化上下文对象解析视频文件元信息
        ret = avformat_find_stream_info(input_context, nullptr);
        if (ret < 0) 
        {
            _last_error = "解析媒体流信息失败: " + GetAVError(ret);
            ERROR("{}", _last_error);
            ret = -1;
            goto cleanup;
        }

        // 3. 创建输出格式化上下文对象
        avformat_alloc_output_context2(&output_context, nullptr, nullptr, output.c_str());
        if (!output_context) 
        {
            _last_error = "创建输出上下文失败";
            ERROR("{}", _last_error);
            ret = -1;
            goto cleanup;
        }

        // 4. 遍历输入格式上下文中的媒体流信息
        for (int i = 0; i < input_context->nb_streams; i++)
        {
            AVStream* input_stream = input_context->streams[i];
            AVStream* output_stream = avformat_new_stream(output_context, nullptr);
            if (!output_stream) 
            {
                _last_error = "创建输出流失败！";
                ERROR("{}", _last_error);
                ret = -1;
                goto cleanup;
            }
            // 复制信息
            ret = avcodec_parameters_copy(output_stream->codecpar, input_stream->codecpar);
            if (ret < 0)
            {
                _last_error = "复制编解码参数失败: " + GetAVError(ret);
                ERROR("{}", _last_error);
                ret = -1;
                goto cleanup;
            }
            output_stream->codecpar->codec_tag = 0;
        }

        // 5. 设置HLS转码的各项字典参数细节
        //  将硬编码的参数，替换为 _settings 里的动态配置
        av_dict_set_int(&dict, "hls_time", _settings.hls_time, 0);
        
        // 防御性编程：如果业务层没填这俩参数，就不设进字典里
        if (!_settings.base_url.empty()) 
        {
            av_dict_set(&dict, "hls_base_url", _settings.base_url.c_str(), 0);
        }
        if (!_settings.playlist_type.empty()) 
        {
            av_dict_set(&dict, "hls_playlist_type", _settings.playlist_type.c_str(), 0);
        }

        // 5.5 检查并打开物理文件
        if (!(output_context->oformat->flags & AVFMT_NOFILE)) 
        {
            ret = avio_open(&output_context->pb, output.c_str(), AVIO_FLAG_WRITE);
            if (ret < 0) 
            {
                _last_error = "打开输出物理文件失败: " + GetAVError(ret);
                ERROR("{}", _last_error);
                ret = -1;
                goto cleanup;
            }
        }

        // 6. 向输出格式化上下文的头部信息写入细节参数
        ret = avformat_write_header(output_context, &dict);
        if (ret < 0)
        {
            _last_error = "向格式化上下文头部信息写入参数失败: " + GetAVError(ret);
            ERROR("{}", _last_error);
            ret = -1;
            goto cleanup;
        }

        // 7. 遍历输入格式化上下文中的数据帧, 进行时间基转换, 从输入媒体流时间基转换为输出媒体流时间基
        while(av_read_frame(input_context, &pkt) >= 0)
        {
            AVStream* input_stream = input_context->streams[pkt.stream_index];
            AVStream* output_stream = output_context->streams[pkt.stream_index];
            
            if (pkt.pts == AV_NOPTS_VALUE)
            {
                pkt.pts = 0;
                pkt.dts = 0;
            }
            // 1. 将数据包中的时间戳, 从输入流的时间基转换为输出流的时间基
            av_packet_rescale_ts(&pkt, input_stream->time_base, output_stream->time_base);
            // 2. 将数据帧通过输出格式化上下文对象, 写入输出文件中
            ret = av_interleaved_write_frame(output_context, &pkt);
            if (ret < 0)
            {
                _last_error = "写入数据帧失败: " + GetAVError(ret);
                ERROR("{}", _last_error);
                av_packet_unref(&pkt);
                break;
            }
            // 3. 释放数据帧
            av_packet_unref(&pkt);
        }

        // 8. 向输出文件写入文件尾部信息
        // 注意：如果在上面的 while 循环里 break 了，写尾部可能会失败，所以也要捕捉错误
        ret = av_write_trailer(output_context);
        if (ret < 0)
        {
            _last_error = "输出尾部信息失败: " + GetAVError(ret);
            ERROR("{}", _last_error);
            ret = -1;
            goto cleanup;
        }

    cleanup:
        // 9. 清理资源
        if (dict) 
        {
            av_dict_free(&dict); 
        }
        if (output_context && !(output_context->oformat->flags & AVFMT_NOFILE)) 
        {
            avio_closep(&output_context->pb);
        }
        if (output_context) 
        {
            avformat_free_context(output_context);
        }
        if (input_context) 
        {
            avformat_close_input(&input_context);
        }
        // ret 如果是 0 代表成功，小于 0 代表失败
        return ret >= 0; 
    
    }

    static std::string GetAVError(int errnum)
    {
        char buf[256]{0};
        av_strerror(errnum, buf, sizeof(buf) - 1);
        return std::string(buf);
    }
}