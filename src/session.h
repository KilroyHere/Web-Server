#ifndef SESSION_H
#define SESSION_H

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include "http_request.h"
#include "request_parser.h"
#include "http_request.h"
#include "http_response.h"

using boost::asio::ip::tcp;

class session
{
public:
  session(boost::asio::io_service &io_service);
  tcp::socket &get_socket();
  void start();

private:
  void read_body(int read_from, int bytes_transferred, int content_length);
  void handle_read(const boost::system::error_code &error, size_t bytes_transferred);
  void handle_write(const boost::system::error_code &error);

  tcp::socket socket_;
  size_t max_buffer_size = 4096;
  std::vector<char> data_;
  request_parser parser_;
  request request_;
  response response_;
};

#endif // SESSION_H