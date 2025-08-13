/*************************************************************************
	> File Name: src/server/tcp_connection.cpp
	> Author: lingye
	> Mail: 15735353673@163.com 
	> Created Time: Mon 07 Jul 2025 09:08:11 PM CST
 ************************************************************************/

#include<iostream>
#include"server/tcp_connection.h"
#include<rtc_base/zmalloc.h>
namespace xrtc
{

	TCPConnection::TCPConnection(int fd):fd(fd),querybuf(sdsempty())
	{

	}

	TCPConnection::~TCPConnection()
	{
		sdsfree(querybuf);
		while(!reply_list.empty())
		{
			rtc::Slice reply = reply_list.front();
			zfree((void*)reply.data());
			reply_list.pop_front();
		}
		reply_list.clear();
	}

}// namespace  xrtc
