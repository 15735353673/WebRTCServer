/*************************************************************************
	> File Name: src/stream/push_stream.h
	> Author: lingye
	> Mail: 15735353673@163.com 
	> Created Time: Fri 11 Jul 2025 10:22:35 PM CST
 ************************************************************************/

#ifndef _PUSH_STREAM_H_
#define _PUSH_STREAM_H_
#include<iostream>
#include"base/event_loop.h"
#include<string>
#include"stream/rtc_stream.h"
namespace xrtc
{

	class PushStream:public RtcStream
	{
	public:
		PushStream(EventLoop* el, PortAllocator* allocator, uint64_t uid, const std::string& stream_name,
				bool audio, bool video, uint32_t log_id);
		~PushStream() override;
		std::string create_offer() override ;
		RtcStreamType stream_type() override {return RtcStreamType::k_push;}

		bool get_audio_source(std::vector<StreamParams>& source);
		bool get_video_source(std::vector<StreamParams>& source);
	private:
		bool _get_source(const std::string& mid,std::vector<StreamParams>& source);
	};
}//namespace xrtc
#endif //_PUSH_STREAM_H_
