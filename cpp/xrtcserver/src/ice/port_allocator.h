/*************************************************************************
	> File Name: port_allocator.h
	> Author: lingye
	> Mail: 15735353673@163.com 
	> Created Time: Tue 15 Jul 2025 12:09:38 PM CST
 ************************************************************************/


#ifndef _PORT_ALLOCATOR_H_
#define _PORT_ALLOCATOR_H_
#include<iostream>
#include"base/network.h"
#include<vector>
#include<memory>

namespace xrtc
{

	class PortAllocator
	{
	public:
		PortAllocator();
		~PortAllocator();

		const std::vector<NetWork*>& get_network(); 

		void set_port_range(int min_port,int max_port);
		int min_port() {return _min_port;}
		int max_port() {return _max_port;}

	private:
		std::unique_ptr<NetWorkManager> _network_manager;
		int _min_port = 0;
		int _max_port = 0;
	};
}//namespace xrtc
#endif //_PORT_ALLOCATOR_H_
