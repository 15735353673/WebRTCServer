/************************************************************************
	> File Name: socket.h
	> Author: lingye
	> Mail: 15735353673@163.com 
	> Created Time: Fri 04 Jul 2025 10:29:38 PM CST
 ************************************************************************/

#ifndef _BASE_SOCKET_H
#define _BASE_SOCKET_H
#include<iostream>
#include<sys/socket.h>

namespace xrtc
{
	int create_tcp_server(const char * addr, int port);
	int create_udp_socket(int family);
	int tcp_accept(int fd, char * host, int * port);
	int sock_setnonblock(int fd);
	int sock_setnodelay(int fd);
	int sock_peer_to_str(int fd,char* ip,int* port);
	int sock_read_data(int fd,char * buf,size_t len);
	int sock_write_data(int fd,const char* buf,size_t len);
	int sock_bind(int sock,struct sockaddr* addr, socklen_t len,int min_port,int max_port);
	int sock_get_address(int sock, char * p, int * port);
	int sock_recv_from(int sock,char * buf,size_t len,struct sockaddr* addr,socklen_t addr_len);
	int64_t sock_get_recv_timestamp(int sock);
	int sock_send_to(int sock, const char* buf,size_t len,int flag,
			struct sockaddr* addr,socklen_t addr_len);
}//namespace xrtc
#endif //_BASE_SOCKET_H
