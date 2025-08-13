/*************************************************************************
	> File Name: src/pc/stream_params.cpp
	> Author: lingye
	> Mail: 15735353673@163.com 
	> Created Time: Sun 03 Aug 2025 03:40:24 PM CST
 ************************************************************************/

#include<iostream>
#include"pc/stream_params.h"

namespace xrtc
{

	SsrcGroup::SsrcGroup(const std::string& semantics,const std::vector<uint32_t>& ssrcs)
		:semantics(semantics),ssrcs(ssrcs)
	{

	}

	bool StreamParams::has_ssrc(uint32_t ssrc)
	{
		for(auto item : ssrcs)
		{
			if(item ==  ssrc)
			{
				return true;
			}
		}

		return false;
	}

}//namespace xrtc
