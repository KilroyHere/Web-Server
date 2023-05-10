#ifndef SESSION_H
#define SESSION_H

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <boost/log/trivial.hpp>
#include <boost/lexical_cast.hpp>
#include "request_handler.h"
#include "config_parser.h"

using boost::asio::ip::tcp;

class session
{
public:
  //Tests
  friend class SessionTest;
  
  ~session();
  session(boost::asio::io_service &io_service, NginxConfig config);
  // TODO: Old Constructor. No Action required. Need to remove eventually.
  // session(boost::asio::io_service &io_service);
  // Gets the socket associated with this session
  tcp::socket &get_socket();
  // Read from the socket
  void async_read();
  // Write to the socket
  void async_write(std::vector<char> response);

private:
  // Callback for async_read, handles the read data
  int handle_read(const boost::system::error_code &error, size_t bytes_transferred);
  // Callback for async_write
  int handle_write(const boost::system::error_code &error);

  tcp::socket socket_;
  size_t max_buffer_size = 1024;
  std::vector<char> data_;
  RequestHandler request_handler_;
  NginxConfig config_;
};

#endif // SESSION_H