/*************************************************************************
	> File Name: src/stream/push_stream.h
	> Author: lingye
	> Mail: 15735353673@163.com 
	> Created Time: Fri 11 Jul 2025 10:22:35 PM CST
 ************************************************************************/

#ifndef _PULL_STREAM_H_
#define _PULL_STREAM_H_
#include<iostream>
#include"base/event_loop.h"
#include<string>
#include"stream/rtc_stream.h"
namespace xrtc
{

	class PullStream:public RtcStream
	{
	public:
		PullStream(EventLoop* el, PortAllocator* allocator, uint64_t uid, const std::string& stream_name,
				bool audio, bool video, uint32_t log_id);
		~PullStream() override;
		std::string create_offer() override ;
		RtcStreamType stream_type() override {return RtcStreamType::k_pull;}

		void add_audio_source(const std::vector<StreamParams>& source);
		void add_video_source(const std::vector<StreamParams>& source);
	};
}//namespace xrtc
#endif //_PULL_STREAM_H_
