#include "http_server.h"

HTTPserver::HTTPserver(boost::asio::io_service &io_service, short port)
    : io_service_(io_service),
      acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
{

  start_accept();
  io_service_.run();
}

void HTTPserver::start_accept()
{
  session *new_session = new session(io_service_);
  acceptor_.async_accept(new_session->get_socket(),
                         boost::bind(&HTTPserver::handle_accept, this, new_session,
                                     boost::asio::placeholders::error));
}

void HTTPserver::handle_accept(session *new_session, const boost::system::error_code &error)
{
  if (!error)
  {
    new_session->start();
  }
  else
  {
    delete new_session;
  }
  // Starts a session and looks for new connections to be accepted
  start_accept();
}