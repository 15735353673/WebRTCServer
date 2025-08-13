/*************************************************************************
	> File Name: xhead.h
	> Author: lingye
	> Mail: 15735353673@163.com 
	> Created Time: Tue 08 Jul 2025 11:54:39 AM CST
 ************************************************************************/
#ifndef _BASE_XHEAD_H_
#define _BASE_XHEAD_H_
#include<iostream>
#include<stdint.h>

namespace xrtc
{
	const int XHEAD_SIZE = 36;
	const uint32_t XHEAD_MAGIC_NUM=0xfb202507;

	struct xhead_t 
	{
		uint16_t id;
		uint16_t version;
		uint32_t log_id;
		char provider[16];
		uint32_t magic_num;
		uint32_t reserved;
		uint32_t body_len;
	};

}//namespace xrtc
#endif //_BASE_XHEAD_H_
