#include <iostream>
#include "TalkToSvr.h"
#define MEM_FN(x) boost::bind(&self_type::x, shared_from_this())
#define MEM_FN1(x,y) boost::bind(&self_type::x, shared_from_this(), y)
#define MEM_FN2(x,y,z) boost::bind(&self_type::x, shared_from_this(),y,z)


talk_to_svr::talk_to_svr(const std::string & message) : sock_(service), started_(true), message_(message) {}
	void talk_to_svr::start(ip::tcp::endpoint ep)
	{
		sock_.async_connect(ep, MEM_FN1(on_connect, _1));
	}

	typedef boost::system::error_code error_code;
	typedef boost::shared_ptr<talk_to_svr> ptr;
	ptr talk_to_svr::start(ip::tcp::endpoint ep, const std::string & message)
	{
		ptr new_(new talk_to_svr(message));
		new_->start(ep);
		return new_;
	}
	void talk_to_svr::stop()
	{
		if (!started_)
			return;
		started_ = false;
		sock_.close();
	}
	bool talk_to_svr::started()
	{
		return started_;
	}

	void talk_to_svr::do_read()
	{
		async_read(sock_, buffer(read_buffer_), MEM_FN2(read_complete, _1, _2), MEM_FN2(on_read, _1, _2));
	}
	void talk_to_svr::do_write(const std::string & msg)
	{
		if (!started())
			return;
		std::copy(msg.begin(), msg.end(), write_buffer_);
		sock_.async_write_some(buffer(write_buffer_, msg.size()), MEM_FN2(on_write, _1, _2));
	}
	size_t talk_to_svr::read_complete(const boost::system::error_code & err, size_t bytes)
	{
		if (err)
			return 0;
		bool found = std::find(read_buffer_, read_buffer_ + bytes, '\n') < read_buffer_ + bytes;  //?????????
		return found ? 0 : 1;
	}

	void talk_to_svr::on_connect(const error_code & err)
	{
		if (!err)
		{
			do_write(message_ + "\n");
		}
		else
		{
			std::cout << "no connection" << std::endl;    //---------------
			stop();
		}
	}
	void talk_to_svr::on_read(const error_code & err, size_t bytes)
	{
		if (!err)
		{
			std::string copy(read_buffer_, bytes - 1);
			std::cout << "server echoed our " << message_ << ": " << (copy == message_ ? "OK" : "FAIL") << std::endl;
		}
		stop();
	}
	void talk_to_svr::on_write(const error_code & err, size_t bytes)
	{
		do_read();
	}
