/*************************************************************************
	> File Name: signaling_server.h
	> Author: lingye
	> Mail: 15735353673@163.com 
	> Created Time: Fri 04 Jul 2025 07:13:27 PM CST
 ************************************************************************/
#ifndef _SERVER_SIGNAL_SERVER_H
#define _SERVER_SIGNAL_SERVER_H


#include<iostream>
#include<string>
#include"base/event_loop.h"
#include<thread>
#include<vector>
#include "server/signaling_worker.h"
#include"server/signaling_server_options.h"
namespace xrtc
{

class SignalingWorker;

class SignalingServer 
{
public:
	enum
	{
		QUIT=0
	};
	SignalingServer();
	~SignalingServer();
	bool start();
	void stop();
	void join();

	int notify(int msg);
	
	int init(const char * conf_file);
	friend void signaling_server_recv_notify(EventLoop* el,IOWatcher* w,int fd,int events,void* data);

	friend void accept_new_conn(EventLoop* el,IOWatcher* w,int fd,int events,void* data);
private:
	void _stop();
	void _process_notify(int msg);
	int _create_worker(int worker_id);
	void _dispatch_new_conn(int fd);
private:
	SignalingServerOptions _options;
	EventLoop* _el=nullptr;
	IOWatcher* _io_watcher=nullptr;
	IOWatcher* _pipe_watcher=nullptr;
	int _notify_recv_fd=-1;
	int _notify_send_fd=-1;
	std::thread* _thread=nullptr;
	int _next_worker_index=0;

	std::vector<SignalingWorker*> _workers;

	int _listen_fd=-1;
};


}//namespace xrtc
#endif //_SERVER_SIGNAL_SERVER_H
