/*************************************************************************
	> File Name: src/pc/stream_params.h
	> Author: lingye
	> Mail: 15735353673@163.com 
	> Created Time: Sun 03 Aug 2025 03:37:25 PM CST
 ************************************************************************/
#ifndef _PC_STREAM_PARAMS_H_
#define _PC_STREAM_PARAMS_H_
#include<iostream>
#include<string>
#include<vector>
namespace xrtc
{

	struct SsrcGroup
	{	
		SsrcGroup(const std::string& semantics,const std::vector<uint32_t>& ssrcs);

		std::string semantics;
		std::vector<uint32_t> ssrcs;

	};

	struct StreamParams
	{
		bool has_ssrc(uint32_t ssrc);

		std::string id;
		std::vector<uint32_t> ssrcs;
		std::vector<SsrcGroup> ssrc_groups;
		std::string cname;
		std::string stream_id;

	};

}//namespace xrtc
#endif// _PC_STREAM_PARAMS_H_
