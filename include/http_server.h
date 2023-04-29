#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "session.h"
#include "config_parser.h"

using boost::asio::ip::tcp;
class HTTPserver
{
public:
  HTTPserver(NginxConfig config, boost::asio::io_service &io_service);

private:
  void start_accept();
  void handle_accept(session *new_session, const boost::system::error_code &error);
  void set_acceptor();


  short port_;
  NginxConfig config_;
  boost::asio::io_service &io_service_;
  tcp::acceptor acceptor_;
  
};

#endif // HTTP_SERVER_H