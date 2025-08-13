/*************************************************************************
	> File Name: ice_credentials.cpp
	> Author: lingye
	> Mail: 15735353673@163.com 
	> Created Time: Sun 13 Jul 2025 09:06:10 PM CST
 ************************************************************************/
#include"ice/ice_credentials.h"
#include"ice/ice_def.h"
#include<iostream>
#include<rtc_base/helpers.h>

namespace xrtc
{

	IceParameters IceCredentials::create_random_ice_credentials()
	{
		return IceParameters(rtc::CreateRandomString(ICE_UFRAG_LENGTH),
				rtc::CreateRandomString(ICE_PWD_LENGTH));
	}
}//namespace xrtc
