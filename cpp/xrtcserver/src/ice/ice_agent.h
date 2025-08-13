/*************************************************************************
	> File Name: ice_agent.h
	> Author: lingye
	> Mail: 15735353673@163.com 
	> Created Time: Mon 14 Jul 2025 12:51:40 PM CST
 ************************************************************************/
#ifndef _ICE_AGENT_H_
#define _ICE_AGENT_H_
#include<iostream>
#include<vector>
#include<string>
#include"base/event_loop.h"
#include"ice/ice_def.h"
#include"ice/ice_transport_channel.h"
namespace xrtc
{
	class IceAgent: public sigslot::has_slots<>
	{
	public:
		IceAgent(EventLoop* el, PortAllocator* allocator);
		~IceAgent();
		bool create_channel(EventLoop* el, const std::string& transport_name,
			IceCandidateComponent component);
		IceTransportChannel* get_channel(const std::string& transport_name,
				IceCandidateComponent component);

		void set_ice_param(const std::string& transport_name,
				IceCandidateComponent component,
				const IceParameters& ice_params);
		
		void set_remote_ice_param(const std::string& transport_name,
				IceCandidateComponent component,
				const IceParameters& ice_params);

		void gathering_candidate();

		IceTransportChannelState ice_state() {return _ice_state;}

		sigslot::signal4<IceAgent*, const std::string&, IceCandidateComponent,
			const std::vector<Candidate>&> signal_candidate_allocate_done;
		sigslot::signal2<IceAgent*, IceTransportChannelState> signal_ice_state;
	private:
		void on_candidate_allocator_done(IceTransportChannel* channel,const std::vector<Candidate>&);
		void _on_ice_receiving_state(IceTransportChannel* channel);
		void _on_ice_writable_state(IceTransportChannel* channel);
		void _on_ice_state_changed(IceTransportChannel* channel);
		void _update_state();
	private:
		std::vector<IceTransportChannel*>::iterator _get_channel(const std::string& transport_name,
				IceCandidateComponent component);
	private:
		EventLoop* _el;
		std::vector<IceTransportChannel*> _channels;
		PortAllocator* _allocator=nullptr;
		IceTransportChannelState _ice_state = IceTransportChannelState::k_new;
	};

}//namespace xrtc
#endif //_ICE_AGENT_H_
