/************************************************************************
	> File Name: rtc_worker.h
	> Author: lingye
	> Mail: 15735353673@163.com 
	> Created Time: Thu 10 Jul 2025 09:40:32 PM CST
 ************************************************************************/


#ifndef _RTC_WORKER_H_
#define _RTC_WORKER_H_
#include<iostream>
#include"server/signaling_server_options.h"
#include"base/event_loop.h"
#include"server/xrtcserver_def.h"
#include"base/lock_free_queue.h"
#include"stream/rtc_stream_manager.h"
#include<memory>

#include<unistd.h>
#include<thread>
namespace xrtc
{

class RtcWorker
{
public:
	enum
	{
		QUIT=0,
		RTC_MSG=1
	};

	RtcWorker(int worker_id,SignalingServerOptions options);
	~RtcWorker();

	int init();
	int start();
	void stop();
	void join();

	int notify(int msg);

	int send_rtc_msg(std::shared_ptr<RtcMsg> msg);
	void push_msg(std::shared_ptr<RtcMsg> msg);
	bool pop_msg(std::shared_ptr<RtcMsg>* msg);

	friend void rtc_worker_recv_notify(EventLoop* /*el*/, IOWatcher* /*w*/, int fd, int /*events*/, void* data);
private:
	void _process_notify(int msg);
	void _stop();
	void _process_rtc_msg();
	void _process_push(std::shared_ptr<RtcMsg> msg);
	void _process_pull(std::shared_ptr<RtcMsg> msg);
	void _process_answer(std::shared_ptr<RtcMsg> msg);
	void _process_stop_push(std::shared_ptr<RtcMsg> msg) ;
	void _process_stop_pull(std::shared_ptr<RtcMsg> msg) ;
	
private:
	SignalingServerOptions _options;
	int _worker_id;
	EventLoop* _el=nullptr;
	IOWatcher* _pipe_watcher=nullptr;

	int _notify_recv_fd=-1;
	int _notify_send_fd=-1;

	std::thread* _thread=nullptr;

	LockFreeQueue<std::shared_ptr<RtcMsg>> _q_msg;

	std::unique_ptr<RtcStreamManager> _rtc_stream_mgr;
};

}//namespace xrtc
#endif //_RTC_WORKER_H_
