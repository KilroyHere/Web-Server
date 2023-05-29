#include <cstdlib>
#include <iostream>
#include <signal.h>
#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include "config_parser.h"
#include "logging.h"
#include "http_server.h"
#include "request_handler_factory.h"
#include "request_handler.h"
#include <boost/beast/http.hpp>

namespace beast = boost::beast;
namespace http = beast::http;

using boost::asio::ip::tcp;

int main(int argc, char *argv[])
{
  Logging::init_log();
  signal(SIGINT, Logging::termintion_log);

  try
  {
    if (argc != 2)
    {
      std::cerr << "Usage: ./server <path to config file>\n";
      if (argc < 2)
      {
        BOOST_LOG_TRIVIAL(error) << "Too few arguments to start server.";
        BOOST_LOG_TRIVIAL(info) << "Server not started.";
      }
      else
      {
        BOOST_LOG_TRIVIAL(error) << "Too many arguments to start server.";
        BOOST_LOG_TRIVIAL(info) << "Server not started.";
      }
      return 1;
    }
    NginxConfigParser config_parser;
    NginxConfig config;
    if (!config_parser.Parse(argv[1], &config))
    {
      BOOST_LOG_TRIVIAL(error) << "Invalid config file, server not started.";
      return 1;
    }
    BOOST_LOG_TRIVIAL(info) << "Valid config file.";

    boost::asio::io_service io_service;
    BOOST_LOG_TRIVIAL(info) << "Trying to start the server.";
    auto server = HTTPserver(config, io_service, 12);
  }
  catch (std::exception &e)
  {
    BOOST_LOG_TRIVIAL(error) << "Exception: " << e.what();
  }

  return 0;
}
