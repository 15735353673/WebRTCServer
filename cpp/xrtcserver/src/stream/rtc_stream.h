/*************************************************************************
	> File Name: src/stream/rtc_stream.h
	> Author: lingye
	> Mail: 15735353673@163.com 
	> Created Time: Fri 11 Jul 2025 10:13:11 PM CST
 ************************************************************************/
#ifndef _RTC_STREAM_H_
#define _RTC_STREAM_H_
#include<iostream>
#include<string>
#include"base/event_loop.h"
#include"pc/peer_connection.h"
#include<memory>
#include<rtc_base/rtc_certificate.h>
#include"ice/port_allocator.h"
namespace xrtc
{

	enum class RtcStreamType
	{
		k_push,
		k_pull

	};
	class RtcStream;

	class RtcStreamListener
	{
	public:
		virtual void on_connection_state(RtcStream* stream , PeerConnectionState state ) =0;
		virtual void on_rtp_packet_received(RtcStream* stream, const char* data, size_t len) = 0;
		virtual void on_rtcp_packet_received(RtcStream* stream, const char* data, size_t len) = 0;
		virtual void on_stream_exception(RtcStream* stream) = 0;
			
	};

	class RtcStream : public sigslot::has_slots<>
	{
	public:
		RtcStream(EventLoop* el,PortAllocator* allocator, uint64_t uid,const std::string& stream_name,
				bool audio, bool video, uint32_t log_id);
		virtual ~RtcStream();

		void register_listener(RtcStreamListener* listener) {_listener = listener;}

		const std::string& get_stream_name() {return stream_name;}
		uint64_t get_uid() {return uid;}

		int start(rtc::RTCCertificate* certificate);
		virtual std::string create_offer() = 0;
		virtual RtcStreamType stream_type() =0;
		int set_remote_sdp(const std::string& sdp);

		int send_rtp(const char* data,size_t len);
		int send_rtcp(const char* data,size_t len);

		std::string to_string();
	private:
		void _on_connection_state(PeerConnection*, PeerConnectionState state);
		void _on_rtp_packet_received(PeerConnection*,
			rtc::CopyOnWriteBuffer* packet, int64_t /*ts*/);
		void _on_rtcp_packet_received(PeerConnection*,
            rtc::CopyOnWriteBuffer* packet, int64_t /*ts*/);
	protected:
		friend class RtcStreamManager;
		EventLoop* el;
		uint64_t uid;
		std::string stream_name;
		bool audio;
		bool video;
		uint32_t log_id;

		PeerConnection* pc;

	private:
		 TimerWatcher* _ice_timeout_watcher = nullptr;
		PeerConnectionState _state = PeerConnectionState::k_new;
		RtcStreamListener* _listener = nullptr;

		friend class RtcStreamManager;
		 friend void ice_timeout_cb(EventLoop* el, TimerWatcher* w, void* data);
	};
}// namespace xrtc
#endif // _RTC_STREAM_H_
