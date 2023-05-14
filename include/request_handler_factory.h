#ifndef REQUEST_HANDLER_FACTORY_H
#define REQUEST_HANDLER_FACTORY_H

#include <boost/filesystem/fstream.hpp>
#include <boost/log/trivial.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include "config_parser.h"
#include "mime_types.h"
#include <iostream>
#include <vector>
#include <unordered_map>
#include <sys/stat.h>
#include <boost/filesystem.hpp>
#include "new_request_handler.h"

#include <boost/beast/http.hpp>

namespace beast = boost::beast;
namespace http = beast::http;  

class RequestHandlerFactory
{
public:
  RequestHandlerFactory(NginxConfig config);

  std::unique_ptr<NewRequestHandler> createHandler(const http::request<http::string_body> *http_request);

private:

  void set_path_root_map();
  void set_path_handler_map();

  NginxConfigParser parser;
  NginxConfig parsed_config;
  std::unordered_map<std::string, std::string> path_root_map_;
  std::unordered_map<std::string, std::string> path_handler_map_;
};

#endif