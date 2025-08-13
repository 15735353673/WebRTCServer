/*************************************************************************
	> File Name: src/server/signaling_server_options.h
	> Author: lingye
	> Mail: 15735353673@163.com 
	> Created Time: Tue 08 Jul 2025 09:37:18 PM CST
 ************************************************************************/

#ifndef _SIGNALING_SERVER_OPTIONS_H_
#define _SIGNALING_SERVER_OPTIONS_H_


#include<iostream>
namespace xrtc
{
struct SignalingServerOptions
{
	std::string host;
	int port;
	int worker_num;
	int connection_timeout;
};

}//namespace std;
#endif// _SIGNALING_SERVER_OPTIONS_H_
