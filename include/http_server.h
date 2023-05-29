#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include "session.h"
#include "config_parser.h"
#include <boost/thread.hpp>
#include <thread>

using boost::asio::ip::tcp;
class HTTPserver
{
public:
  HTTPserver(NginxConfig config, boost::asio::io_service &io_service, uint32_t thread_pool_size = 1);

private:
  uint32_t thread_pool_size_;
  void start_accept();
  void handle_accept(session *new_session, const boost::system::error_code &error);
  void set_acceptor();
  void handle_read(session *new_session);
  void run();
  short port_;
  NginxConfig config_;
  boost::asio::io_service &io_service_;
  tcp::acceptor acceptor_;
};

#endif // HTTP_SERVER_H