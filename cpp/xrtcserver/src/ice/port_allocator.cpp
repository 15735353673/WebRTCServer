/*************************************************************************
	> File Name: port_allocator.cpp
	> Author: lingye
	> Mail: 15735353673@163.com 
	> Created Time: Tue 15 Jul 2025 12:22:13 PM CST
 ************************************************************************/

#include<iostream>
#include"ice/port_allocator.h"

namespace xrtc
{

	PortAllocator::PortAllocator():
		_network_manager(new NetWorkManager())
	{
		_network_manager->create_networks();

	}

	PortAllocator::~PortAllocator()
	{

	}

	const std::vector<NetWork*>& PortAllocator::get_network()
	{
		return _network_manager->get_networks();
	}

	void PortAllocator::set_port_range(int min_port,int max_port)
	{
		if (min_port > 0)
		{
		    _min_port = min_port;
		}

		if (max_port > 0) 
		{
		    _max_port = max_port;
		}
	}

}//namespace xrtc
