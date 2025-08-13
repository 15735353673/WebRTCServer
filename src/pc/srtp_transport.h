/*************************************************************************
	> File Name: src/pc/srtp_transport.h
	> Author: lingye
	> Mail: 15735353673@163.com 
	> Created Time: Sun 03 Aug 2025 07:46:54 PM CST
 ************************************************************************/
#ifndef _SRTP_TRANSPORT_H_
#define _SRTP_TRANSPORT_H_
#include<iostream>
#include<vector>
#include<memory>
#include"pc/srtp_session.h"
#include <rtc_base/third_party/sigslot/sigslot.h>
namespace xrtc
{
	class SrtpTransport : public sigslot::has_slots<>
	{
	public:
		SrtpTransport(bool rtcp_mux_enabled);
		virtual ~SrtpTransport() = default;
		bool set_rtp_params(int send_cs,
			const uint8_t* send_key,
			size_t send_key_len,
			const std::vector<int>& send_extension_ids,
			int recv_cs,
			const uint8_t* recv_key,
			size_t recv_key_len,
			const std::vector<int>& recv_extension_ids
			);
		void reset_params();
		bool is_srtp_active();
		bool unprotect_rtp(void * p,int in_len,int* out_len);
		bool unprotect_rtcp(void * p,int in_len,int* out_len);
		bool protect_rtp(void* p, int in_len, int max_len, int* out_len);
		bool protect_rtcp(void* p, int in_len, int max_len, int* out_len);
		void get_send_auth_tag_len(int* rtp_auth_tag_len, int* rtcp_auth_tag_len);
	private:
		void _create_srtp_session();
	protected:
		bool _rtcp_mux_enabled;
		std::unique_ptr<SrtpSession> _send_session = nullptr;
		std::unique_ptr<SrtpSession> _recv_session = nullptr;
	};

}//namespace xrtc
#endif //_SRTP_TRANSPORT_H_
