/*************************************************************************
	> File Name: signaling_server.cpp
	> Author: lingye
	> Mail: 15735353673@163.com 
	> Created Time: Fri 04 Jul 2025 07:18:17 PM CST
 ************************************************************************/

#include<iostream>
#include<rtc_base/logging.h>
#include<yaml-cpp/yaml.h>
#include"server/signaling_server.h"
#include"base/socket.h"
#include<unistd.h>

namespace xrtc
{

		
	
	void signaling_server_recv_notify(EventLoop* /*el*/,IOWatcher* /*w*/,int fd,int /*events*/,void* data)
	{
		//收到事件触发回调
		int msg;
		if(read(fd,&msg,sizeof(int)) !=sizeof(int))
		{
			RTC_LOG(LS_WARNING) << "read from pipe error: "<< strerror(errno)
				<<", errno:" << errno;
			return;
		}
		SignalingServer* server = (SignalingServer*) data;
		server->_process_notify(msg);
	}

	void accept_new_conn(EventLoop* /*el*/,IOWatcher* /*w*/,int fd,int /*events*/,void* data)
	{
			int cfd;
			char cip[128];
			int cport;

			cfd = tcp_accept(fd,cip,&cport);
			if(cfd == -1)
			{
				return ;
			}


			RTC_LOG(LS_INFO) << "accept new conn, fd: " << cfd << ", ip: " << cip
				<< ", port: " << cport;


			//分发worker
			SignalingServer* server=(SignalingServer*)data;
			server->_dispatch_new_conn(cfd);


	}

	void SignalingServer::_dispatch_new_conn(int fd)
	{
		int index=_next_worker_index;
		_next_worker_index++;
		if((size_t)_next_worker_index >= _workers.size())
		{
			_next_worker_index=0;
		}

		SignalingWorker* worker=_workers[index];
		worker->notify_new_conn(fd);
	}

	SignalingServer::SignalingServer():_el(new EventLoop(this))
	{

	}

	SignalingServer::~SignalingServer()
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

	int SignalingServer::_create_worker(int worker_id)
	{
		RTC_LOG(LS_INFO) << "create worker worker_id:"<<worker_id;
		SignalingWorker* worker=new SignalingWorker(worker_id,_options);

		if(worker->init() !=0)
		{
			return -1;
		}
		
		if(worker->start() !=0)
		{
			return -1;
		}

		_workers.push_back(worker);
		return 0;
	}

	int SignalingServer::init(const char * conf_file)
	{
		if(!conf_file)
		{
			RTC_LOG(LS_WARNING) << "signaling server conf_file is null";
			return -1;
		}

		try
		{
			YAML::Node config = YAML::LoadFile(conf_file);

			_options.host =config["signalingServer"]["host"].as<std::string>();
			_options.port =config["signalingServer"]["port"].as<int>();
			_options.worker_num =config["signalingServer"]["worker_num"].as<int>();
			_options.connection_timeout =config["signalingServer"]["connection_timeout"].as<int>();
		} catch (YAML::Exception& e)
		{
			RTC_LOG(LS_WARNING) << "catch a YAML exception,line:" << e.mark.line+1
				<< ",column:"<<e.mark.column+1 <<",error"<<e.msg;
			return -1;
		}


		//创建管道 用于线程间通信
		int fds[2];
		if(pipe(fds))
		{
			RTC_LOG(LS_WARNING) << "create pipe failed："<<strerror(errno)<<", errno:"<<errno;
			return -1;
		}
		_notify_recv_fd=fds[0];
		_notify_send_fd=fds[1];
		//将recv_fd纳入事件循环
		_pipe_watcher=_el->create_io_event(signaling_server_recv_notify,this);
		_el->start_io_event(_pipe_watcher,_notify_recv_fd,EventLoop::READ);

		//创建Tcp server
		_listen_fd = create_tcp_server(_options.host.c_str(),_options.port);
		if(-1 == _listen_fd)
		{
			return -1;
		}
		//将这个fd纳入事件循环
		_io_watcher= _el->create_io_event(accept_new_conn,this);
		_el->start_io_event(_io_watcher,_listen_fd,EventLoop::READ);

		//创建worker
		for(int i=0;i<_options.worker_num;i++)
		{
			if(_create_worker(i) != 0)
			{
				return -1;
			}
		}

		return 0;
	}

	bool SignalingServer::start()
	{
		if(_thread)
		{
			RTC_LOG(LS_WARNING) << "signaling server already start";
			return false;
		}
		
		_thread=new std::thread([=](){		
					RTC_LOG(LS_INFO) << "signaling server event loop run";
					//阻塞
					_el->start();
					RTC_LOG(LS_INFO) << "signaling server event loop stop";
				});
		return true;
	}

	void SignalingServer::stop()
	{
		//通过管道发送消息来完成事件循环的停止
		notify(SignalingServer::QUIT);
	}

	int SignalingServer::notify(int msg)
	{
		int written=write(_notify_send_fd,&msg,sizeof(int));
		return written==sizeof(int) ?0 : -1;
	}

	void SignalingServer::_process_notify(int msg)
	{
		switch (msg) {
			case QUIT:
				_stop();
				break;
			default:
				RTC_LOG(LS_WARNING) << "unknown msg: " << msg;
				break;
    }
	}

	void SignalingServer::_stop()
	{
		if(!_thread)
		{
			RTC_LOG(LS_WARNING) << "signaling server not running";
			return ;
		}

		_el->delete_io_event(_pipe_watcher);
		_el->delete_io_event(_io_watcher);
		_el->stop();

		close(_listen_fd);
		close(_notify_recv_fd);
		close(_notify_send_fd);
		
		for(auto worker:_workers)
		{
			if(worker)
			{
				worker->stop();
				worker->join();
			}
		}

		RTC_LOG(LS_INFO) << "signaling server stop";
	}


	void SignalingServer::join()
	{
		if(_thread && _thread->joinable())
		{
			_thread->join();
		}
	}


}//namespace xrtc
