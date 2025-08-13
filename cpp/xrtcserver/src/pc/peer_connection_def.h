/*************************************************************************
	> File Name: src/pc/peer_connection_def.h
	> Author: lingye
	> Mail: 15735353673@163.com 
	> Created Time: Fri 01 Aug 2025 04:46:01 PM CST
 ************************************************************************/
#ifndef _PEER_CONNETION_DEF_H_
#define _PEER_CONNETION_DEF_H_
#include<iostream>

namespace xrtc 
{

enum class PeerConnectionState
{
	k_new,
	k_connecting,
	k_connected,
	k_disconnected,
	k_failed,
	k_closed
};

}
#endif // _PEER_CONNETION_DEF_H_
