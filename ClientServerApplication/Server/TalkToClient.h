#ifndef TALKTOCLIENT_H
#define TALKTOCLIENT_H

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

using namespace boost::asio;

extern io_service service;

class talk_to_client : public boost::enable_shared_from_this<talk_to_client>, boost::noncopyable
{
	typedef talk_to_client self_type;
	talk_to_client();

public:
	typedef boost::system::error_code error_code;
	typedef boost::shared_ptr<talk_to_client> ptr;
	void start();
	static ptr new_();
	void stop();
	ip::tcp::socket & sock();
	bool started();

	void do_read();
	void do_write(const std::string & msg);
	size_t read_complete(const boost::system::error_code & err, size_t bytes);

	void on_read(const error_code & err, size_t bytes);
	void on_write(const error_code & err, size_t bytes);

private:
	ip::tcp::socket sock_;
	enum { max_msg = 1024 };
	char read_buffer_[max_msg];
	char write_buffer_[max_msg];
	bool started_;
};

#endif