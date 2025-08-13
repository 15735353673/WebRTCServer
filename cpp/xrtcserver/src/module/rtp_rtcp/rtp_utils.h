/*************************************************************************
	> File Name: rtp_utils.h
	> Author: lingye
	> Mail: 15735353673@163.com 
	> Created Time: Mon 04 Aug 2025 12:48:28 PM CST
 ************************************************************************/
#ifndef _MODULE_RTP_UTILS_H_
#define _MODULE_RTP_UTILS_H_
#include<iostream>
#include <api/array_view.h>

namespace xrtc
{
	enum class RtpPacketType
	{
		k_rtp,
		k_rtcp,
		k_unknown,
	};

	RtpPacketType infer_rtp_packet_type(rtc::ArrayView<const char> packet);
	uint16_t parse_rtp_sequence_number(rtc::ArrayView<const uint8_t> packet);
	uint32_t parse_rtp_ssrc(rtc::ArrayView<const uint8_t> packet);
	bool get_rtcp_type(const void* data,size_t len, int* type);

}//namespace xrtc
#endif //_MODULE_RTP_UTILS_H_
