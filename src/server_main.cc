#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include "http_server.h"
#include "config_parser.h"

using boost::asio::ip::tcp;

int main(int argc, char *argv[])
{
  try
  {
    if (argc != 2)
    {
      std::cerr << "Usage: ./server <path to config file>\n";
      return 1;
    }
    NginxConfigParser config_parser;
    NginxConfig config;
    if (!config_parser.Parse(argv[1], &config))
    {
      return 1;
    }

    boost::asio::io_service io_service;
    HTTPserver s(config, io_service);
  }
  catch (std::exception &e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
