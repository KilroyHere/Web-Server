#ifndef NEW_REQUEST_HANDLER_H
#define NEW_REQUEST_HANDLER_H

#include <iostream>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/log/trivial.hpp>
#include "mime_types.h"
#include "config_parser.h"
#include <boost/lexical_cast.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/http/status.hpp>

namespace beast = boost::beast;
namespace http = beast::http;

class NewRequestHandler
{
public:
  virtual bool handle_request(http::request<http::string_body> *http_request, http::response<http::string_body> *http_response) = 0;
  // bool connection_close(const http::request<std::string> *http_request);
  //virtual ~NewRequestHandler() = 0;  
};

class EchoRequestHandler : public NewRequestHandler
{
public:
  EchoRequestHandler(const std::string &request_uri, NginxConfig &config);
  bool handle_request(http::request<http::string_body> *http_request, http::response<http::string_body> *http_response) override;
};

class StaticRequestHandler : public NewRequestHandler
{
public:
  StaticRequestHandler(const std::string &request_uri, NginxConfig &config);
  bool handle_request( http::request<http::string_body> *http_request, http::response<http::string_body> *http_response) override;
};

class BadRequestHandler : public NewRequestHandler
{
public:
  BadRequestHandler(const std::string &request_uri, NginxConfig &config);
  bool handle_request(http::request<http::string_body> *http_request, http::response<http::string_body> *http_response) override;
};

class NotFoundRequestHandler : public NewRequestHandler
{
public:
  NotFoundRequestHandler(const std::string &request_uri, NginxConfig &config);
  bool handle_request(http::request<http::string_body> *http_request, http::response<http::string_body> *http_response) override;
};

#endif // RESPONSE_HANDLER_H