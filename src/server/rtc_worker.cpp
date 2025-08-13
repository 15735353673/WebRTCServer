/*************************************************************************
	> File Name: rtc_worker.cpp
	> Author: lingye
	> Mail: 15735353673@163.com 
	> Created Time: Thu 10 Jul 2025 09:45:00 PM CST
 ************************************************************************/

#include<iostream>
#include"server/rtc_worker.h"
#include<rtc_base/logging.h>
#include"server/signaling_worker.h"
namespace xrtc
{

	RtcWorker::RtcWorker(int worker_id, SignalingServerOptions options):
		_options(options),
		_worker_id(worker_id),
		_el(new EventLoop(this)),
		_rtc_stream_mgr(std::make_unique<RtcStreamManager>(_el))
	{

	}

	RtcWorker::~RtcWorker()
	{
		if(_el)
		{
			delete _el;
			_el=nullptr;
		}

		if(_thread)
		{
			delete _thread;
			_thread=nullptr;
		}
	}

	void rtc_worker_recv_notify(EventLoop* /*el*/, IOWatcher* /*w*/, int fd, int /*events*/, void* data)
	{
		int msg;
		if(read(fd,&msg, sizeof(msg)) != sizeof(int))
		{
			RTC_LOG(LS_WARNING) << "read from pipe error :" <<strerror(errno)
				<<", errno:" << errno;
			return ;
		}

		//处理消息
		RtcWorker* worker = (RtcWorker*)data;
		worker->_process_notify(msg);
	}
	
	int RtcWorker::send_rtc_msg(std::shared_ptr<RtcMsg> msg)
	{
		push_msg(msg);
		notify(RTC_MSG);
		return 0;
	}

	void RtcWorker::push_msg(std::shared_ptr<RtcMsg> msg)
	{
		_q_msg.produce(msg);
	}

	bool  RtcWorker::pop_msg(std::shared_ptr<RtcMsg>* msg)
	{
		return _q_msg.consume(msg);

	}

	int RtcWorker::init()
	{
		//创建管道
		int fds[2];
		if(pipe(fds))
		{
			RTC_LOG(LS_WARNING) << "create pipe error :" << strerror(errno)
				<<", errno:" << errno;
			return -1;
		}

		_notify_recv_fd=fds[0];
		_notify_send_fd=fds[1];

		_pipe_watcher=_el->create_io_event(rtc_worker_recv_notify,this);
		_el->start_io_event(_pipe_watcher,_notify_recv_fd,EventLoop::READ);

		return 0;
	}

	int RtcWorker::start()
	{
		if(_thread)
		{
			RTC_LOG(LS_WARNING) << "rtc worker alreadly, worker_id:" << _worker_id;
			return -1;
		}
		_thread= new std::thread([=](){
					RTC_LOG(LS_INFO) << "rtc worker event loop start ,worker_id:"<< _worker_id;
					_el->start();
					RTC_LOG(LS_INFO) << "rtc worker event loop stop ,worker_id:" << _worker_id;
				});
		return 0;
	}

	void RtcWorker::stop()
	{
		notify(QUIT);
	}

	int RtcWorker::notify(int msg)
	{
		int writeen = write(_notify_send_fd, &msg, sizeof(int));
		return writeen == sizeof(int) ? 0 : -1;
	}

	void RtcWorker:: join()
	{
		if(_thread && _thread->joinable())
		{
			_thread->join();
		}
	}

	void RtcWorker::_stop()
	{

		if(!_thread)
		{
			RTC_LOG(LS_WARNING) << "rtc worker not running, worker_id:"<< _worker_id;
			return ;
		}
		_el->delete_io_event(_pipe_watcher);
		_el->stop();
		close(_notify_recv_fd);
		close(_notify_send_fd);
	}

	void RtcWorker::_process_push(std::shared_ptr<RtcMsg> msg)
	{
		std::string offer;
		 int ret = _rtc_stream_mgr->create_push_stream(msg->uid,msg->stream_name,
				msg->audio,msg->video,msg->log_id,(rtc::RTCCertificate*)(msg->certificate),offer);

		RTC_LOG(LS_INFO) << "offer:" << offer;

		msg->sdp = offer;


		if(ret != 0)
		{
			msg->err_no = -1;
		}
		//把消息发送给signaling_worker进行处理
		SignalingWorker* worker= (SignalingWorker*)msg->worker;
		if(worker)
		{
			worker->send_rtc_msg(msg);
		}

	}

	void RtcWorker::_process_pull(std::shared_ptr<RtcMsg> msg)
	{
		std::string offer;
		 int ret = _rtc_stream_mgr->create_pull_stream(msg->uid,msg->stream_name,
				msg->audio,msg->video,msg->log_id,(rtc::RTCCertificate*)(msg->certificate),offer);

		RTC_LOG(LS_INFO) << "offer:" << offer;

		msg->sdp = offer;


		if(ret != 0)
		{
			msg->err_no = -1;
		}
		//把消息发送给signaling_worker进行处理
		SignalingWorker* worker= (SignalingWorker*)msg->worker;
		if(worker)
		{
			worker->send_rtc_msg(msg);
		}

	}
	
	void RtcWorker::_process_answer(std::shared_ptr<RtcMsg> msg)
	{
		int ret= _rtc_stream_mgr->set_answer(msg->uid,msg->stream_name,
				msg->sdp,msg->stream_type,msg->log_id);

		RTC_LOG(LS_INFO) << "rtc worker process answer, uid:" << msg->uid
			<<", stream_name:" << msg->stream_name 
			<<", worker_id:" <<_worker_id
			<<", log_id" << msg->log_id
			<<", ret:" <<ret;
	}
	

	void RtcWorker::_process_stop_push(std::shared_ptr<RtcMsg> msg) 
	{
		int ret = _rtc_stream_mgr->stop_push(msg->uid, msg->stream_name);


		RTC_LOG(LS_INFO) << "rtc worker process stop push, uid: " << msg->uid
			<< ", stream_name: " << msg->stream_name
			<< ", worker_id: " << _worker_id
			<< ", log_id: " << msg->log_id
			<< ", ret: " << ret;
	}

	void RtcWorker::_process_stop_pull(std::shared_ptr<RtcMsg> msg) 
	{
		int ret = _rtc_stream_mgr->stop_pull(msg->uid, msg->stream_name);


		RTC_LOG(LS_INFO) << "rtc worker process stop pull, uid: " << msg->uid
			<< ", stream_name: " << msg->stream_name
			<< ", worker_id: " << _worker_id
			<< ", log_id: " << msg->log_id
			<< ", ret: " << ret;
	}

	void RtcWorker::_process_rtc_msg()
	{
		std::shared_ptr<RtcMsg> msg;
		if(!pop_msg(&msg))
		{
			return ;
		}

		RTC_LOG(LS_INFO) << "cmdno[" << msg->cmdno << "] uid[" << msg->uid
			<< "] stream_name[" << msg->stream_name << "] audio[" << msg->audio
			<< "] video[" << msg->video << "] log_id[" << msg->log_id
			<< "] rtc worker receive msg, worker_id: " 
			<< _worker_id;

		switch(msg->cmdno)
		{
			case CMDNO_PUSH:
				_process_push(msg);
				break;
			case CMDNO_PULL:
				_process_pull(msg);
				break;
			case CMDNO_STOP_PUSH:
				_process_stop_push(msg);
				break;
			case CMDNO_STOPPULL:
				_process_stop_pull(msg);
				break;
			case CMDNO_ANSWER:
				_process_answer(msg);
				break;
			default:
				RTC_LOG(LS_WARNING) << "unknown cndno:" << msg->cmdno 
					<<", log_id:" << msg->log_id;
				break;
		}
	}

	void RtcWorker::_process_notify(int msg)
	{
		switch(msg)
		{
			case QUIT:
				_stop();
				break;
			case RTC_MSG:
				_process_rtc_msg();
				break;
			default:
				RTC_LOG(LS_WARNING) << "unknown msg" << msg;
		}
	}


}//namespace xrtc
