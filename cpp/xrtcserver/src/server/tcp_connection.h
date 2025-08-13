/*************************************************************************
	> File Name: src/server/tcp_connection.h
	> Author: lingye
	> Mail: 15735353673@163.com 
	> Created Time: Mon 07 Jul 2025 08:31:47 PM CST
 ************************************************************************/
#ifndef _TCP_CONNECTION_H_
#define _TCP_CONNECTION_H_

#include<iostream>
#include"base/event_loop.h"
#include<rtc_base/sds.h>
#include"base/xhead.h"
#include<list>
#include<rtc_base/slice.h>

namespace xrtc
{
	class TCPConnection
	{
	public:
		enum 
		{
			STATE_HEAD=0,
			STATE_BODY=1
		};
		TCPConnection(int);
		~TCPConnection();
	public:
		int fd;
		char ip[64];
		int port;
		IOWatcher* io_watcher=nullptr;
		TimerWatcher* timer_watcher =  nullptr;

		sds querybuf;
		int current_state=STATE_HEAD;
		size_t bytes_expected = XHEAD_SIZE;//期待读取的数据
		size_t bytes_processed=0;//已经处理的数据
		unsigned long last_interaction = 0;

		std::list<rtc::Slice> reply_list;
		size_t cur_resp_pos=0;
	};

}//namespace xrtc
#endif //_TCP_CONNECTION_H_
