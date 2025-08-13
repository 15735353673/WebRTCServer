/*************************************************************************
	> File Name: rtc_server.cpp
	> Author: lingye
	> Mail: 15735353673@163.com 
	> Created Time: Thu 10 Jul 2025 06:26:20 PM CST
 ************************************************************************/

#include<iostream>
#include"server/rtc_server.h"
#include"server/rtc_worker.h"
#include<yaml-cpp/yaml.h>
#include<rtc_base/logging.h>
#include<unistd.h>
#include<rtc_base/crc32.h>
#include<rtc_base/rtc_certificate_generator.h>

namespace xrtc
{

	const uint64_t k_year_in_ms = 365*24*3600*1000L;
	RtcServer::RtcServer():_el(new EventLoop(this))
	{

	}

	RtcServer::~RtcServer()
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

		for(auto worker:_workers)
		{
			if(worker)
			{
				delete worker;
			}
		}

		_workers.clear();
	}

	int RtcServer::_gengerate_and_check_certificate()
	{
		if(!_certificate || _certificate->HasExpired(time(NULL)*1000))
		{
			rtc::KeyParams key_params;
			RTC_LOG(LS_WARNING) << "dtls enable, key type : " << key_params.type();
			_certificate = rtc::RTCCertificateGenerator::GenerateCertificate(key_params,
					k_year_in_ms);
			if(_certificate)
			{
				rtc::RTCCertificatePEM pem = _certificate->ToPEM();
				RTC_LOG(LS_INFO) << "rtc certificate : \n" << pem.certificate();
			}
		}

		if(!_certificate)
		{
			RTC_LOG(LS_WARNING) << "get certificate error";
			return -1;
		}

		return 0;
	}

	void rtc_server_recv_notify(EventLoop* /*el*/, IOWatcher* /*w*/, int fd, int /*events*/, void * data)
	{
		int msg;
		if(read(fd,&msg,sizeof(int)) != sizeof(int))
		{
			RTC_LOG(LS_WARNING) << "rtc server read from pipe error: " << strerror(errno)
				<<", errno: " << errno;
			return ;
		}

		//处理消息
		RtcServer* server=(RtcServer*) data;
		server->_process_notify(msg);
	}


	int RtcServer::init(const char* filename)
	{
		if(!filename)
		{
			RTC_LOG(LS_WARNING) << "conf_file  name is null";
			return -1;
		}

		try
		{
			YAML::Node config=YAML::LoadFile(filename);
			_options.worker_num = config["rtcServer"]["worker_num"].as<int>(); 
			RTC_LOG(LS_INFO) << "rtc server options:\n" << config;
		}
		catch(YAML::Exception& e)
		{
			RTC_LOG(LS_WARNING) << "rtc server load conf file failed ， error:" << e.msg;
			return -1;
		}

		//生成证书
		if(_gengerate_and_check_certificate() != 0 )
		{
			return -1;
		}

		//创建管道
		int fds[2];
		if(pipe(fds))
		{
			RTC_LOG(LS_WARNING) << "create pipe error: " << strerror(errno)
				<<", errno:" << errno;
			return -1;
		}

		_notify_recv_fd=fds[0];
		_notify_send_fd=fds[1];

		//启动事件循环
		_pipe_watcher = _el->create_io_event(rtc_server_recv_notify, this);
		_el->start_io_event(_pipe_watcher, _notify_recv_fd, EventLoop::READ);

		for(int i=0;i<_options.worker_num;i++)
		{
			if(_create_worker(i) != 0)
			{
				return -1;
			}
		}

		return 0;
	}

	int RtcServer::_create_worker(int worker_id)
	{
		RTC_LOG(LS_INFO) << "rtc server create worker, worker_id:" << worker_id;
		RtcWorker* worker=new RtcWorker(worker_id,_options);
		
		if(worker->init() !=0)
		{
			return -1;
		}
		
		
		if(worker->start())
		{
			return -1;
		}
		
		
		_workers.push_back(worker);


		return 0;
	}

	int RtcServer::start()
	{
		if(_thread)
		{
			RTC_LOG(LS_WARNING) << "rtc server event loop already start";
			return -1;
		}

		_thread = new std::thread([=](){
				RTC_LOG(LS_INFO) << "rtc server event loop start ";
				_el->start();
				RTC_LOG(LS_INFO) << "rtc server ebent loop stop";
				});

		return 0;
	}

	void RtcServer::stop()
	{
		notify(QUIT);
	}

	int RtcServer::notify(int msg)
	{
		int written = write(_notify_send_fd,&msg,sizeof(msg));
		return written == sizeof(int) ? 0 : -1;
	}

	void RtcServer::join()
	{
		if(_thread && _thread->joinable())
		{
			_thread->join();
		}
	}

	void RtcServer::_stop()
	{
		_el->delete_io_event(_pipe_watcher);
		_el->stop();
		close(_notify_recv_fd);
		close(_notify_send_fd);

		for(auto worker : _workers)
		{
			if(worker)
			{
				worker->stop();
				worker->join();
			}
		}
	}

	void RtcServer::_process_rtc_msg()
	{
		std::shared_ptr<RtcMsg> msg = pop_msg();
		if(!msg)
		{
			return ;
		}
		
		if(_gengerate_and_check_certificate() != 0)
		{
			return ;
		}

		msg->certificate = _certificate.get();

		//将消息分发给worker
		RtcWorker* worker = _get_worker(msg->stream_name);
		if(worker)
		{
			worker->send_rtc_msg(msg);
		}

	}

	RtcWorker* RtcServer::_get_worker(const std::string& stream_name)
	{
		if(_workers.size() ==0 || _workers.size() != (size_t)_options.worker_num)
		{
			return nullptr;
		}

		uint32_t num = rtc::ComputeCrc32(stream_name);
		size_t index = num % _options.worker_num;
		return _workers[index];
	}

	int RtcServer::send_rtc_msg(std::shared_ptr<RtcMsg> msg)
	{
		//将msg插入到消息队列 并发送消息
		push_msg(msg);
		return notify(RTC_MSG);
	}

	void RtcServer::push_msg(std::shared_ptr<RtcMsg> msg)
	{
		std::unique_lock<std::mutex> lock(_q_msg_mtx);
		_q_msg.push(msg);

	}

	std::shared_ptr<RtcMsg> RtcServer::pop_msg()
	{
		std::unique_lock<std::mutex> lock(_q_msg_mtx);

		if(_q_msg.empty())
		{
			return nullptr;
		}

		std::shared_ptr<RtcMsg> msg = _q_msg.front();
		_q_msg.pop();
		return msg;

	}

	void RtcServer::_process_notify(int msg)
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
