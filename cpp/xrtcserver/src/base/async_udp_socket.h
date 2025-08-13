/*************************************************************************
	> File Name: async_udp_socket.h
	> Author: lingye
	> Mail: 15735353673@163.com 
	> Created Time: Sat 19 Jul 2025 03:55:27 PM CST
 ************************************************************************/
#ifndef _ASYNC_UDP_SOCKET_H_
#define _ASYNC_UDP_SOCKET_H_
#include<iostream>
#include"base/event_loop.h"
#include<rtc_base/socket_address.h>
#include <rtc_base/third_party/sigslot/sigslot.h>
#include<list>
namespace xrtc
{



	class UdpPacketData
	{
	public:
		UdpPacketData(const char* data,size_t size,const rtc::SocketAddress& addr):_data(new char[size]),_size(size),_addr(addr){memcpy(_data,data,size);}
		~UdpPacketData()
		{
			if(_data)
			{
				delete [] _data;
				_data = nullptr;
			}
		}
		char* data() {return _data;}
		size_t size() {return _size;}
		const rtc::SocketAddress& addr() {return _addr;}
	private:
		char* _data;
		size_t _size;
		rtc::SocketAddress _addr;


	};

	class AsyncUdpSocket
	{
	public:
		AsyncUdpSocket(EventLoop* el,int socket);
		void recv_data();
		~AsyncUdpSocket();

		int send_to(const char* data,size_t len,const rtc::SocketAddress& addr);
		
		void send_data();

		sigslot::signal5<AsyncUdpSocket*, char*, size_t, const rtc::SocketAddress&, int64_t>
			signal_read_packet;

	private:
		int _add_udp_packet(const char* data,size_t size,const rtc::SocketAddress& addr);
	private:
		EventLoop* _el=nullptr;
		IOWatcher* _socket_watcher = nullptr;
		int _socket = -1;
		char* _buf = nullptr;
		int _size;

		std::list<UdpPacketData*> _udp_packet_list;
	};


}//namespace xrtc
#endif //_ASYNC_UDP_SOCKET_H_
