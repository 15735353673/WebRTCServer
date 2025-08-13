/*************************************************************************
	> File Name: log.h
	> Author: lingye
	> Mail: 15735353673@163.com 
	> Created Time: Thu 03 Jul 2025 03:39:07 PM CST
 ************************************************************************/

#ifndef _BASE_LOG_H_
#define _BASE_LOG_H_

#include <rtc_base/logging.h>
#include<queue>
#include<fstream>
#include<thread>
#include<mutex>

namespace xrtc
{
class XrtcLog : public rtc::LogSink 
{
public:
	XrtcLog(const std::string& log_dir,
			const std::string& log_name,
			const std::string& log_level);
	~XrtcLog() override;

	int init(); 
	bool start();
	void stop();
	void join();

	void set_log_to_stderr(bool on);
	void OnLogMessage(const std::string& message, rtc::LoggingSeverity severity) override;
	void OnLogMessage(const std::string& message) override;
private:
	std::string _log_dir;
	std::string _log_name;
	std::string _log_level;
	std::string _log_file;
	std::string _log_file_wf;

	std::ofstream _out_file;
	std::ofstream _out_file_wf;

	std::queue<std::string> _log_queue;
	std::queue<std::string> _log_queue_wf;

	std::mutex _mtx;
	std::mutex _mtx_wf;

	std::thread* _thread=nullptr;
	std::atomic<bool> _running{false};
};

}//namespace xrtc

#endif // _BASE_LOG_H_
