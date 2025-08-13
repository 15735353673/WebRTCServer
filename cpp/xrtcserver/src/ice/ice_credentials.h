/*************************************************************************
	> File Name: ice_credentials.h
	> Author: lingye
	> Mail: 15735353673@163.com 
	> Created Time: Sun 13 Jul 2025 09:00:15 PM CST
 ************************************************************************/
#ifndef _ICE_CREDENTIALS_H_
#define _ICE_CREDENTIALS_H_

#include<iostream>
#include<string>
namespace xrtc
{

	struct IceParameters
	{
		IceParameters()= default;
		IceParameters(const std::string& ufrag,const std::string& pwd):
			ice_ufrag(ufrag),ice_pwd(pwd)
		{

		}

		std::string ice_ufrag;
		std::string ice_pwd;
	};

	class IceCredentials
	{
	public:
		static IceParameters create_random_ice_credentials();
	};
}//namespace xrtc

#endif //_ICE_CREDENTIALS_H_
