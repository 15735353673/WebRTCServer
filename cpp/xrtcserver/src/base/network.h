/*************************************************************************
	> File Name: network.h
	> Author: lingye
	> Mail: 15735353673@163.com 
	> Created Time: Mon 14 Jul 2025 09:18:19 PM CST
 ************************************************************************/
#ifndef _BASE_NETWORK_H_
#define _BASE_NETWORK_H_
#include<iostream>
#include<rtc_base/ip_address.h>
#include<string>
#include<vector>
namespace xrtc
{
	class NetWork
	{
	public:
		NetWork(const std::string& name , const rtc::IPAddress& ip):
			_name(name),_ip(ip){}
		~NetWork() = default;
		
		const std::string& name() {return _name;}
		const rtc::IPAddress ip() {return _ip;}
		const rtc::IPAddress ip_public();

		std::string to_string(){return _name+":" + _ip.ToString();}
	private:
		std::string _name;
		rtc::IPAddress _ip;
	};

	class NetWorkManager
	{
	public:
		NetWorkManager();
		~NetWorkManager();
		
		const std::vector<NetWork*>& get_networks() {return _network_list;}
		int create_networks();

	private:
		std::vector<NetWork*> _network_list;
	};

}//namespace xrtc
#endif //_BASE_NETWORK_H_
