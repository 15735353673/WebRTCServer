/*************************************************************************
	> File Name: src/ice/ice_connection_info.h
	> Author: lingye
	> Mail: 15735353673@163.com 
	> Created Time: Wed 30 Jul 2025 08:25:58 PM CST
 ************************************************************************/
#ifndef _ICE_CONNECTION_INFO_H_
#define _ICE_CONNECTION_INFO_H_
#include<iostream>
namespace xrtc
{
	enum class IceCandidatePairState 
	{
		WAITING, // 连通性检查尚未开始
		IN_PROGRESS, // 检查进行中
		SUCCEEDED, // 检查成功
		FAILED, // 检查失败
	};
	
}//namespace xrtc
#endif //_ICE_CONNECTION_INFO_H_
