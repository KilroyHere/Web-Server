#include "http_server.h"

using boost::asio::ip::tcp;

HTTPserver::HTTPserver(NginxConfig config, boost::asio::io_service &io_service)
    : config_(config), io_service_(io_service), acceptor_(io_service)
{
 
  std::vector<std::string> query{"server", "listen"};
  std::string port;
  if (!config_.config_port_num(query, port))
  {
    std::cerr << "Port not found in the config file\n";
  }
  else
  {
    port_ = stoi(port);
    set_acceptor();
    std::cerr << "Server listening at Port:" << port_ << '\n';
    start_accept();
    io_service_.run();
  }
}

void HTTPserver::set_acceptor()
{
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port_);
    acceptor_.open(endpoint.protocol());
    acceptor_.bind(endpoint);
    acceptor_.listen();
}

void HTTPserver::start_accept()
{
  session *new_session = new session(io_service_,config_);
  acceptor_.async_accept(new_session->get_socket(),
                         boost::bind(&HTTPserver::handle_accept, this, new_session,
                                     boost::asio::placeholders::error));
}

void HTTPserver::handle_accept(session* new_session, const boost::system::error_code &error)
{
  if (!error)
  {
    new_session->async_read();
  }
  else
  {
    // TODO: Error Log
    delete new_session;
  }
  // Starts a session and looks for new connections to be accepted
  start_accept();
}