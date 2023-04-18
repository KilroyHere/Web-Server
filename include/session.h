#ifndef SESSION_H
#define SESSION_H

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include "request_handler.h"

using boost::asio::ip::tcp;

class session
{
public:
  ~session();
  session(boost::asio::io_service &io_service);
  tcp::socket &get_socket();
  void async_read();
  void async_write(std::vector<char> response);

private:
  void read_body(int read_from, int bytes_transferred, int content_length);
  void handle_read(const boost::system::error_code &error, size_t bytes_transferred);
  void handle_write(const boost::system::error_code &error);

  tcp::socket socket_;
  size_t max_buffer_size = 1024;
  std::vector<char> data_;
  RequestHandler request_handler_;
};

#endif // SESSION_H