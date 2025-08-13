/*************************************************************************
	> File Name: network.cpp
	> Author: lingye
	> Mail: 15735353673@163.com 
	> Created Time: Mon 14 Jul 2025 09:26:16 PM CST
 ************************************************************************/

#include"base/network.h"
#include<iostream>
#include<ifaddrs.h>
#include<rtc_base/logging.h>

namespace xrtc
{

	NetWorkManager::NetWorkManager()
	{

	}

	NetWorkManager::~NetWorkManager()
	{
		for(auto network:_network_list)
		{
			if(network)
			{
				delete network;
			}
		}

		_network_list.clear();
	}

	const rtc::IPAddress NetWork::ip_public()
	{
		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;

		// 将点分十进制字符串转换为二进制形式
		inet_pton(AF_INET, "101.126.155.67", &addr.sin_addr);

		// 转换为rtc::IPAddress
		return rtc::IPAddress(addr.sin_addr);
	}
		
	int NetWorkManager::create_networks()
	{
		struct ifaddrs* interface;
		int err = getifaddrs(&interface);
		if(err != 0)
		{
			RTC_LOG(LS_WARNING) << "getifaddrs error:" << strerror(errno)
				<<", errno" << errno;
			return -1;
		}

		for(auto cur = interface; cur != nullptr; cur=cur->ifa_next)
		{
			if(cur->ifa_addr->sa_family != AF_INET)
			{
				continue;
			}

			struct sockaddr_in* addr = (struct sockaddr_in*) (cur->ifa_addr);
			rtc::IPAddress ip_address(addr->sin_addr);

			if ( rtc::IPIsLoopback(ip_address)) 
			{
				continue;
			}
			NetWork* network = new NetWork(cur->ifa_name,ip_address);
			RTC_LOG(LS_INFO) << "gathered network interface: " << network->to_string();

			_network_list.push_back(network);
		}

		freeifaddrs(interface);

		return 0;
	}
}//namespace xrtc

