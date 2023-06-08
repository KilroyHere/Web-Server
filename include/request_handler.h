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
#include <boost/algorithm/string.hpp>
#include <boost/functional/hash.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/http/status.hpp>
#include <regex>
#include <unistd.h>
#include <mutex>

namespace beast = boost::beast;
namespace http = beast::http;

class RequestHandler
{
public:
  // Fills the response with the necessary information with respect to the handler that called it.
  virtual bool handle_request(const http::request<http::string_body> http_request, http::response<http::string_body> *http_response) = 0;
  // bool connection_close(const http::request<std::string> *http_request);
  virtual std::string get_name() = 0;
  std::mutex mutex_file;
};

class EchoRequestHandler : public RequestHandler
{
public:
  EchoRequestHandler(const std::string &request_uri, NginxConfig &config);
  // Fills the response body with the http request and sets status to ok.
  bool handle_request(const http::request<http::string_body> http_request, http::response<http::string_body> *http_response) override;
  std::string get_name() override;
};

class StaticRequestHandler : public RequestHandler
{
public:
  StaticRequestHandler(const std::string &request_uri, NginxConfig &config);
  // Fills the response body with the contents of the corresponding file and sets status to ok.
  bool handle_request(const http::request<http::string_body> http_request, http::response<http::string_body> *http_response) override;
  std::string get_name() override;

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
  std::string get_name() override;
};

class NotFoundRequestHandler : public RequestHandler
{
public:
  NotFoundRequestHandler(const std::string &request_uri, NginxConfig &config);
  // Keeps the response body empty and sets status to not found.
  bool handle_request(const http::request<http::string_body> http_request, http::response<http::string_body> *http_response) override;
  std::string get_name() override;
};

class CrudRequestHandler : public RequestHandler
{
public:
  CrudRequestHandler(const std::string &request_uri, NginxConfig &config);
  // Keeps the response body empty and sets status to not found.
  bool handle_request(const http::request<http::string_body> http_request, http::response<http::string_body> *http_response) override;
  std::string get_name() override;

private:
  NginxConfig config_;
  std::string data_path;
};

class HealthRequestHandler : public RequestHandler
{
public:
  HealthRequestHandler(const std::string &request_uri, NginxConfig &config);
  // Fills the response body with the http request and sets status to ok.
  bool handle_request(const http::request<http::string_body> http_request, http::response<http::string_body> *http_response) override;
  std::string get_name() override;
};

class SleepRequestHandler : public RequestHandler
{
public:
  SleepRequestHandler(const std::string &request_uri, NginxConfig &config);
  // Sleeps for 4 seconds and then sets status to ok.
  bool handle_request(const http::request<http::string_body> http_request, http::response<http::string_body> *http_response) override;
  std::string get_name() override;
};

class AuthenticationRequestHandler : public RequestHandler
{
public:
  AuthenticationRequestHandler(const std::string &request_uri, NginxConfig &config);

  bool handle_request(const http::request<http::string_body> http_request, http::response<http::string_body> *http_response) override;
  std::string get_name() override;

private:
  NginxConfig config_;
  std::string data_path;
};

#endif // REQUEST_HANDLER_H
