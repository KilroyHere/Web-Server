#include "http_server.h"

using boost::asio::ip::tcp;

HTTPserver::HTTPserver(NginxConfig config, boost::asio::io_service &io_service, uint32_t thread_pool_size /* = 1*/)
    : config_(config), io_service_(io_service), acceptor_(io_service), thread_pool_size_(thread_pool_size)
{

  std::vector<std::string> query{"port"};
  std::string port;
  if (!config_.config_port_num(query, port))
  {
    BOOST_LOG_TRIVIAL(error) << "Port not found in the config file.";
  }
  else
  {
    port_ = stoi(port);
    set_acceptor();
    BOOST_LOG_TRIVIAL(info) << "Server listening at Port: " << port_ << ".";
    start_accept();
    run();
  }
}


void HTTPserver::run()
{
    // Create a pool of threads to run all of the io_services.
    std::vector<boost::shared_ptr<boost::thread> > threads;
    for (std::size_t i = 0; i < thread_pool_size_; ++i)
    {
    boost::shared_ptr<boost::thread> thread(new boost::thread(
            boost::bind(&boost::asio::io_service::run, &io_service_)));
    threads.push_back(thread);
    }

    // Wait for all threads in the pool to exit.
    for (std::size_t i = 0; i < threads.size(); ++i)
    threads[i]->join();
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
  session *new_session = new session(io_service_, config_);
  acceptor_.async_accept(new_session->get_socket(),
                         boost::bind(&HTTPserver::handle_accept, this, new_session,
                                     boost::asio::placeholders::error));
}

void HTTPserver::handle_read(session *new_session)
{
  new_session->async_read();
}

void HTTPserver::handle_accept(session *new_session, const boost::system::error_code &error)
{
  if (!error)
  {
    handle_read(new_session);
  }
  else
  {
    // TODO: Error Log
    delete new_session;
  }
  // Starts a session and looks for new connections to be accepted
  start_accept();
}