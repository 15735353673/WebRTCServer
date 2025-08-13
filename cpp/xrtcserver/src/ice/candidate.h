/*************************************************************************
	> File Name: candidate.h
	> Author: lingye
	> Mail: 15735353673@163.com 
	> Created Time: Tue 15 Jul 2025 06:53:57 PM CST
 ************************************************************************/
#ifndef _CANDIDATE_H_
#define _CANDIDATE_H_
#include<iostream>
#include<string>
#include<rtc_base/socket_address.h>
#include"ice/ice_def.h"
namespace xrtc
{

	class Candidate
	{
	public:
		Candidate();
		~Candidate();
		uint32_t get_priority(uint32_t type_preference, int network_adapter_preference,
				int relay_preference);

		std::string to_string() const;
	public:
		IceCandidateComponent component;
		std::string protocol;
		rtc::SocketAddress address;
		int port;
		uint32_t priority;
		std::string username;
		std::string password;
		std::string type;
		std::string foundation;
	};
}//nbamespace xrtc
#endif // _CANDIDATE_H_
