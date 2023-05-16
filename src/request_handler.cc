#include "request_handler.h"

EchoRequestHandler::EchoRequestHandler(const std::string &request_uri, NginxConfig &config) {}

StaticRequestHandler::StaticRequestHandler(const std::string &request_uri, NginxConfig &config) : config_(config), file_path(request_uri) {}

BadRequestHandler::BadRequestHandler(const std::string &request_uri, NginxConfig &config) {}

NotFoundRequestHandler::NotFoundRequestHandler(const std::string &request_uri, NginxConfig &config) {}

bool EchoRequestHandler::handle_request(const http::request<http::string_body> http_request, http::response<http::string_body> *http_response)
{
  std::ostringstream oss;
  oss << http_request;
  http_response->result(http::status::ok);
  http_response->version(http_request.version());
  http_response->body() = oss.str();
  http_response->set(http::field::content_type, "text/plain");
  http_response->prepare_payload();
  return true;
}
bool StaticRequestHandler::handle_request(const http::request<http::string_body> http_request, http::response<http::string_body> *http_response)
{
  // read file contents
  std::ifstream file(file_path.c_str(), std::ios::in | std::ios::binary);
  if (file.fail())
  {
    BOOST_LOG_TRIVIAL(info) << "Couldn't open file";
    return false;
  }
  BOOST_LOG_TRIVIAL(info) << "Reading the file.";
  std::string body = "";

  char c;
  while (file.get(c))
    body += c;

  BOOST_LOG_TRIVIAL(info) << "Got file contents.";

  file.close();

  BOOST_LOG_TRIVIAL(info) << "File closed.";

  std::string extension;
  size_t cursor = file_path.find_last_of(".");
  if (cursor != std::string::npos)
    extension = file_path.substr(cursor + 1);

  http_response->result(http::status::ok);
  http_response->version(http_request.version());
  http_response->body() = body;
  http_response->set(http::field::content_type, extension_to_type(extension));
  http_response->prepare_payload();
  return true;
}

bool BadRequestHandler::handle_request(const http::request<http::string_body> http_request, http::response<http::string_body> *http_response)
{
  http_response->result(http::status::bad_request);
  http_response->body() = "";
  http_response->prepare_payload();
  return true;
}

bool NotFoundRequestHandler::handle_request(const http::request<http::string_body> http_request, http::response<http::string_body> *http_response)
{
  http_response->result(http::status::not_found);
  http_response->body() = "";
  http_response->prepare_payload();
  return true;
}