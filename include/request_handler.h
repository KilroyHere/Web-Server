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
  // Fills the response with the necessary information with respect to the handler that called it.
  virtual bool handle_request(const http::request<http::string_body> http_request, http::response<http::string_body> *http_response) = 0;
  // bool connection_close(const http::request<std::string> *http_request);
};

class EchoRequestHandler : public RequestHandler
{
public:
  EchoRequestHandler(const std::string &request_uri, NginxConfig &config);
  // Fills the response body with the http request and sets status to ok.
  bool handle_request(const http::request<http::string_body> http_request, http::response<http::string_body> *http_response) override;
};

class StaticRequestHandler : public RequestHandler
{
public:
  StaticRequestHandler(const std::string &request_uri, NginxConfig &config);
  // Fills the response body with the contents of the corresponding file and sets status to ok.
  bool handle_request(const http::request<http::string_body> http_request, http::response<http::string_body> *http_response) override;
private:
  NginxConfig config_;
  std::string file_path;
};

class BadRequestHandler : public RequestHandler
{
public:
  BadRequestHandler(const std::string &request_uri, NginxConfig &config);
  // Keeps the response body empty and sets status to bad request.
  bool handle_request(const http::request<http::string_body> http_request, http::response<http::string_body> *http_response) override;
};

class NotFoundRequestHandler : public RequestHandler
{
public:
  NotFoundRequestHandler(const std::string &request_uri, NginxConfig &config);
  // Keeps the response body empty and sets status to not found.
  bool handle_request(const http::request<http::string_body> http_request, http::response<http::string_body> *http_response) override;
};

class CrudRequestHandler : public RequestHandler
{
public:
  CrudRequestHandler(const std::string &request_uri, NginxConfig &config);
  // Keeps the response body empty and sets status to not found.
  bool handle_request(const http::request<http::string_body> http_request, http::response<http::string_body> *http_response) override;
private:
  NginxConfig config_;
  std::string data_path;
};

#endif // REQUEST_HANDLER_H