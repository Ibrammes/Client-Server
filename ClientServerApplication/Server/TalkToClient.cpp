#include <iostream>
#include "TalkToClient.h"
#define MEM_FN(x) boost::bind(&self_type::x, shared_from_this())
#define MEM_FN1(x,y) boost::bind(&self_type::x, shared_from_this(), y)
#define MEM_FN2(x,y,z) boost::bind(&self_type::x, shared_from_this(),y,z)

	typedef talk_to_client self_type;
	talk_to_client::talk_to_client() : sock_(service), started_(false) {}

	typedef boost::system::error_code error_code;
	typedef boost::shared_ptr<talk_to_client> ptr;
	void talk_to_client::start()
	{
		started_ = true;
		do_read();
	}
	ptr talk_to_client::new_()
	{
		ptr new_(new talk_to_client);
		return new_;
	}
	void talk_to_client::stop()
	{
		if (!started_)
			return;
		started_ = false;
		sock_.close();
	}
	ip::tcp::socket & talk_to_client::sock()
	{
		return sock_;
	}
	bool talk_to_client::started()
	{
		return started_;
	}

	void talk_to_client::do_read()
	{
		async_read(sock_, buffer(read_buffer_), MEM_FN2(read_complete, _1, _2), MEM_FN2(on_read, _1, _2));
	}
	void talk_to_client::do_write(const std::string & msg)
	{
		if (!started())
			return;
		std::copy(msg.begin(), msg.end(), write_buffer_);
		sock_.async_write_some(buffer(write_buffer_, msg.size()), MEM_FN2(on_write, _1, _2));
	}
	size_t talk_to_client::read_complete(const boost::system::error_code & err, size_t bytes)
	{
		if (err)
			return 0;
		bool found = std::find(read_buffer_, read_buffer_ + bytes, '\n') < read_buffer_ + bytes;
		return found ? 0 : 1;
	}

	void talk_to_client::on_read(const error_code & err, size_t bytes)
	{
		if (!err)
		{
			std::string msg(read_buffer_, bytes);
			do_write(msg + "\n");
			std::cout << "received: " << msg;   //-----------------------
		}
		stop();
	}
	void talk_to_client::on_write(const error_code & err, size_t bytes)
	{
		do_read();
	}
