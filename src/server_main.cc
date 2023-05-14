#include <cstdlib>
#include <iostream>
#include <signal.h>
#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include "config_parser.h"
#include "logging.h"
#include "request_handler_factory.h"
#include "new_request_handler.h"
#include <boost/beast/http.hpp>

namespace beast = boost::beast;
namespace http = beast::http; 

using boost::asio::ip::tcp;

int main(int argc, char *argv[])
{
  NginxConfigParser config_parser;
  NginxConfig config;
  if (!config_parser.Parse(argv[1], &config))
  {
    BOOST_LOG_TRIVIAL(error) << "Invalid config file, server not started.";
    return 1;
  }

  RequestHandlerFactory nhf(config);
  http::request<http::string_body> req{http::verb::get, "/echo/index.html", 10};

  std::ostringstream os;
  // os << req;
  // std::cerr << os.str() << std::endl;
  // os.clear();

  std::unique_ptr<NewRequestHandler> res = nhf.createHandler(&req);
  // std::cerr<< req.version() << std::endl;
  http::response<http::string_body> http_response;
  bool result = res->handle_request(&req, &http_response);
  std::ostringstream oss;
  oss << http_response;
  std::cerr << oss.str() << std::endl;
  // Logging::init_log();
  // signal(SIGINT, Logging::termintion_log);

  // try
  // {
  //   if (argc != 2)
  //   {
  //     std::cerr << "Usage: ./server <path to config file>\n";
  //     if (argc < 2)
  //     {
  //       BOOST_LOG_TRIVIAL(error) << "Too few arguments to start server.";
  //       BOOST_LOG_TRIVIAL(info) << "Server not started.";
  //     }
  //     else
  //     {
  //       BOOST_LOG_TRIVIAL(error) << "Too many arguments to start server.";
  //       BOOST_LOG_TRIVIAL(info) << "Server not started.";
  //     }
  //     return 1;
  //   }
  //   NginxConfigParser config_parser;
  //   NginxConfig config;
  //   if (!config_parser.Parse(argv[1], &config))
  //   {
  //     BOOST_LOG_TRIVIAL(error) << "Invalid config file, server not started.";
  //     return 1;
  //   }
  //   BOOST_LOG_TRIVIAL(info) << "Valid config file.";

  //   boost::asio::io_service io_service;
  //   BOOST_LOG_TRIVIAL(info) << "Trying to start the server.";
  //   HTTPserver s(config, io_service);
  // }
  // catch (std::exception &e)
  // {
  //   std::cerr << "Exception: " << e.what() << "\n";
  //   BOOST_LOG_TRIVIAL(error) << "Exception: " << e.what();
  // }

  // return 0;
}
