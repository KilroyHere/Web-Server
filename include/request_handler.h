#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

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

class RequestHandler
{
public:
  virtual bool handle_request(const http::request<http::string_body> http_request, http::response<http::string_body> *http_response) = 0;
  // bool connection_close(const http::request<std::string> *http_request);
  //virtual ~RequestHandler() = 0;  
};

class EchoRequestHandler : public RequestHandler
{
public:
  EchoRequestHandler(const std::string &request_uri, NginxConfig &config);
  bool handle_request(const http::request<http::string_body> http_request, http::response<http::string_body> *http_response) override;
};

class StaticRequestHandler : public RequestHandler
{
public:
  StaticRequestHandler(const std::string &request_uri, NginxConfig &config);
  bool handle_request(const http::request<http::string_body> http_request, http::response<http::string_body> *http_response) override;
private:
  NginxConfig config_;
  std::string file_path;
};

class BadRequestHandler : public RequestHandler
{
public:
  BadRequestHandler(const std::string &request_uri, NginxConfig &config);
  bool handle_request(const http::request<http::string_body> http_request, http::response<http::string_body> *http_response) override;
};

class NotFoundRequestHandler : public RequestHandler
{
public:
  NotFoundRequestHandler(const std::string &request_uri, NginxConfig &config);
  bool handle_request(const http::request<http::string_body> http_request, http::response<http::string_body> *http_response) override;
};

#endif // REQUEST_HANDLER_H