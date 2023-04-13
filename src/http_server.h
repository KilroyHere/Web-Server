#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "session.h"
using boost::asio::ip::tcp;
class HTTPserver
{
public:
  HTTPserver(boost::asio::io_service &io_service, short port);

private:
  void start_accept();
  void handle_accept(session *new_session, const boost::system::error_code &error);

  boost::asio::io_service &io_service_;
  tcp::acceptor acceptor_;
};



#endif // HTTP_SERVER_H