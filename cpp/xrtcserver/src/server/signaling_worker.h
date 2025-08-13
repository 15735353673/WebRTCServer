/*************************************************************************
	> File Name: signaling_worker.h
	> Author: lingye
	> Mail: 15735353673@163.com 
	> Created Time: Mon 07 Jul 2025 02:13:31 PM CST
 ************************************************************************/

#ifndef _SIGNAL_WORKER_H
#define _SIGNAL_WORKER_H


#include<iostream>
#include"server/signaling_server_options.h"
#include"base/event_loop.h"
#include"base/lock_free_queue.h"
#include<thread>
#include "server/xrtcserver_def.h"
#include<vector>
#include"server/tcp_connection.h"
#include<rtc_base/slice.h>
#include<json/json.h>
#include"server/rtc_server.h"
#include<queue>
#include<mutex>
#include"server/xrtcserver_def.h"

namespace xrtc
{

	class TCPConnection;

	class SignalingWorker
	{
	public:
		enum
		{
			QUIT=0,
			NEW_CONN=1,
			RTC_MSG=2
		};
		SignalingWorker(int worker_id, const SignalingServerOptions& options);
		~SignalingWorker();
		int init();
		int start();
		void stop();
		void join();
		int notify(int msg);
		int notify_new_conn(int fd);

		int send_rtc_msg(std::shared_ptr<RtcMsg> msg);
		void push_msg(std::shared_ptr<RtcMsg> msg);
		std::shared_ptr<RtcMsg> pop_msg();


		friend void signaling_worker_recv_notify(EventLoop* /*el*/,IOWatcher* w, int fd,int events, void * data );
		friend void conn_io_cb(EventLoop* /*el*/,IOWatcher* /*w*/,int fd, int events,void* data);
		friend void conn_time_cb(EventLoop* el,TimerWatcher* /*w*/, void * data);
	private:
		void _stop();
		void _process_notify(int msg);
		void _new_conn(int fd);
		void _read_query(int fd);
		void _write_reply(int fd);
		int _process_request(TCPConnection* c, const rtc::Slice& header,const rtc::Slice& body);
		int _process_query_buffer(TCPConnection* c);
		void _close_conn(TCPConnection* c);
		void _remove_conn(TCPConnection* c);
		void _process_timeout(TCPConnection* c);
		int _process_push(int cmdno,TCPConnection* c,const Json::Value& root,uint32_t log_id);
		int _process_pull(int cmdno,TCPConnection* c,const Json::Value& root,uint32_t log_id);
		int _process_stop_push(int cmdno, TCPConnection* c, const Json::Value& root, uint32_t log_id);
		int _process_stop_pull(int cmdno, TCPConnection* c, const Json::Value& root, uint32_t log_id);
		int _process_answer(int cmdno,TCPConnection* c,const Json::Value& root,uint32_t log_id);
		void _process_rtc_msg();
		void _response_server_offer(std::shared_ptr<RtcMsg> msg);
		void _add_reply(TCPConnection* c, const rtc::Slice& reply);
	private:
		int _worker_id;
		SignalingServerOptions _options;
		EventLoop* _el=nullptr;
		IOWatcher* _pipe_watcher=nullptr;
		int _notify_recv_fd=-1;
		int _notify_send_fd=-1;
		
		LockFreeQueue<int> _q_conn;
		std::vector<TCPConnection*> _conns;

		std::thread* _thread=nullptr;

		std::queue<std::shared_ptr<RtcMsg>> _q_msg;
		std::mutex _q_msg_mtx;

	};
}//namespace xrtc
#endif //_SIGNAL_WORKER_H
