/*************************************************************************
	> File Name: rtc_server.h
	> Author: lingye
	> Mail: 15735353673@163.com 
	> Created Time: Thu 10 Jul 2025 06:24:14 PM CST
 ************************************************************************/

#ifndef _SERVER_RTC_SERVER_H_
#define _SERVER_RTC_SERVER_H_

#include<iostream>
#include"base/event_loop.h"
#include<thread>
#include"server/xrtcserver_def.h"
#include"server/signaling_server_options.h"
#include<mutex>
#include<queue>
#include<vector>
#include<rtc_base/rtc_certificate.h>

namespace xrtc
{
	class RtcWorker;

	class RtcServer
	{
	public:
		enum
		{
			QUIT = 0,
			RTC_MSG = 1
		};
		RtcServer();
		~RtcServer();
		int init(const char* filename);
		int notify(int msg);

		int start();
		void stop();
		void join();
		
		int send_rtc_msg(std::shared_ptr<RtcMsg> msg);
		void push_msg(std::shared_ptr<RtcMsg> msg);
		std::shared_ptr<RtcMsg> pop_msg();

		friend void rtc_server_recv_notify(EventLoop* /*el*/, IOWatcher* w, int fd, int /*events*/, void * data);
	private:
		void _process_notify(int msg);
		void _stop();
		void _process_rtc_msg();
		int _create_worker(int worker_id);
		RtcWorker* _get_worker(const std::string& stream_name);

		int _gengerate_and_check_certificate();
	private:
		EventLoop* _el=nullptr;
		IOWatcher* _pipe_watcher = nullptr;
		int _notify_recv_fd = -1;
		int _notify_send_fd = -1;

		SignalingServerOptions _options;

		std::queue<std::shared_ptr<RtcMsg>> _q_msg;
		std::mutex _q_msg_mtx;

		std::thread* _thread=nullptr;

		std::vector<RtcWorker*> _workers;

		rtc::scoped_refptr<rtc::RTCCertificate> _certificate;
	};
}//namespace xrtc
#endif //_SERVER_RTC_SERVER_H_
