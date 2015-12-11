#ifndef TALKTOSVR_H
#define TALKTOSVR_H

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

using namespace boost::asio;

extern io_service service;

class talk_to_svr : public boost::enable_shared_from_this<talk_to_svr>, boost::noncopyable
{
	typedef talk_to_svr self_type;
	talk_to_svr(const std::string & message);
	void start(ip::tcp::endpoint ep);

public:
	typedef boost::system::error_code error_code;
	typedef boost::shared_ptr<talk_to_svr> ptr;
	static ptr start(ip::tcp::endpoint ep, const std::string & message);
	void stop();
	bool started();

	void do_read();
	void do_write(const std::string & msg);
	size_t read_complete(const boost::system::error_code & err, size_t bytes);

	void on_connect(const error_code & err);
	void on_read(const error_code & err, size_t bytes);
	void on_write(const error_code & err, size_t bytes);

private:
	ip::tcp::socket sock_;
	enum mm{
		max_msg = 1024
	};
	char read_buffer_[max_msg];
	char write_buffer_[max_msg];
	bool started_;
	std::string message_;
};

#endif