/*************************************************************************
	> File Name: log.cpp
	> Author: lingye
	> Mail: 15735353673@163.com 
	> Created Time: Thu 03 Jul 2025 09:32:16 PM CST
 ************************************************************************/

#include<iostream>
#include"base/log.h"
#include<sys/stat.h>


namespace xrtc 
{

	XrtcLog::XrtcLog(const std::string& log_dir,
			const std::string& log_name,
			const std::string& log_level) :
		_log_dir(log_dir),
		_log_name(log_name),
		_log_level(log_level),
		_log_file(log_dir+ "/"+log_name +".log"),
		_log_file_wf(log_dir + "/" +log_name + ".log.wf")
	{

	}

	XrtcLog::~XrtcLog() 
	{
		stop();
		_out_file.close();
		_out_file_wf.close();
	}

	static rtc::LoggingSeverity get_log_level(const std::string& level)
	{
		if ("verbose" == level) {
			return rtc::LS_VERBOSE;
		} else if ("info" == level) {
			return rtc::LS_INFO;
		} else if ("warning" == level) {
			return rtc::LS_WARNING;
		} else if ("error" == level) {
			return rtc::LS_ERROR;
		} else if ("none" == level) {
			return rtc::LS_NONE;
		}

		return rtc::LS_NONE;
	}

	int XrtcLog::init()
	{
		// 配置日志：显示线程信息和时间戳
		rtc::LogMessage::ConfigureLogging("thread tstamp");
		rtc::LogMessage::SetLogPathPrefix("/src");
		rtc::LogMessage::AddLogToStream(this,get_log_level(_log_level));
		//打开log文件
		int ret = mkdir(_log_dir.c_str(),0755);
		if(ret != 0 && errno != EEXIST) 
		{
			fprintf(stderr,"create log_dir[%s] failed\n",_log_dir.c_str());
			return -1;
		}

		_out_file.open(_log_file,std::ios::app);
		if(!_out_file.is_open())
		{
			fprintf(stderr,"open out_file[%s] failed\n",_log_file.c_str());
			return -1;
		}
		
		_out_file_wf.open(_log_file_wf,std::ios::app);
		if(!_out_file_wf.is_open())
		{
			fprintf(stderr,"open out_file_wf[%s] failed\n",_log_file_wf.c_str());
			return -1;
		}

		return 0;
	}

	bool XrtcLog::start()
	{
		if (_running)
		{
			fprintf(stderr,"log thread already running");
			return false;
		}
		//启动log线程
		_running=true;
		_thread = new std::thread([=](){
				std::stringstream ss;
				struct stat stat_data;
				while(_running)
				{
					//检查日志文件是否被删除或者移动
					if(stat(_log_file.c_str(),&stat_data)<0)
					{
						//关闭文件重新打开
						_out_file.close();
						_out_file.open(_log_file,std::ios::app);
					}
					
					if(stat(_log_file_wf.c_str(),&stat_data)<0)
					{
						//关闭文件重新打开
						_out_file_wf.close();
						_out_file_wf.open(_log_file_wf,std::ios::app);
					}
					
					bool write_log=false;
					{
						std::unique_lock<std::mutex> lock(_mtx);
						if(!_log_queue.empty())
						{
							while(!_log_queue.empty())
							{
								write_log=true;
								ss << _log_queue.front();
								_log_queue.pop();
							}
						}
					}
					if(write_log)
					{
						_out_file << ss.str();
						_out_file.flush();
					}

					ss.str("");
					
					bool write_log_wf=false;
					{
						std::unique_lock<std::mutex> lock(_mtx_wf);
						if(!_log_queue_wf.empty())
						{
							while(!_log_queue_wf.empty())
							{
								write_log_wf=true;
								ss << _log_queue_wf.front();
								_log_queue_wf.pop();
							}
						}
					}
					if(write_log_wf)
					{
						_out_file_wf << ss.str();
						_out_file_wf.flush();
					}

					ss.str("");

					std::this_thread::sleep_for(std::chrono::milliseconds(30));
				}


				});
		return true;
	}

	void XrtcLog::stop()
	{
		_running=false;

		if(_thread)
		{
			if(_thread->joinable()) 
			{
				_thread->join();
			}
		}
		delete _thread;
		_thread=nullptr;
	}

	void XrtcLog::join()
	{
		if(_thread && _thread->joinable())
		{
			_thread->join();
		}
	}

	void XrtcLog::OnLogMessage(const std::string& message, rtc::LoggingSeverity severity)
	{

		//往队列添加
		if (severity >= rtc::LS_WARNING)
		{
			//加锁
			std::unique_lock<std::mutex> lock(_mtx_wf);
			_log_queue_wf.push(message);
		} 
		else
		{
			std::unique_lock<std::mutex> lock(_mtx);
			_log_queue.push(message);
		}
		

	}
	void XrtcLog::OnLogMessage(const std::string& /*message*/)
	{
		//不用实现
	}

	void XrtcLog::set_log_to_stderr(bool on)
	{
		rtc::LogMessage::SetLogToStderr(on);
	}


}//namespace xrtc
