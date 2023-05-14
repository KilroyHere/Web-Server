  // GET /echo/index.html HTTP/1.1 ["", "echo", "index.html"]
  // Host: 34.83.71.241
  // Connection: keep-alive
  // Upgrade-Insecure-Requests: 1
  // User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/113.0.0.0 Safari/537.36
  // Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8
  // Sec-GPC: 1
  // Accept-Language: en-US,en
  // Accept-Encoding: gzip, deflate 
  // Reference

#include "new_request_handler.h"

// bool NewRequestHandler::connection_close(const http::request<std::string> *http_request)
// {
//     //check header connection is set to close -> return false;
//     return true;
// }

EchoRequestHandler::EchoRequestHandler( const std::string &request_uri, NginxConfig &config) {}

StaticRequestHandler::StaticRequestHandler(const std::string &request_uri, NginxConfig &config) {}

BadRequestHandler::BadRequestHandler(const std::string &request_uri, NginxConfig &config) {}

NotFoundRequestHandler::NotFoundRequestHandler(const std::string &request_uri, NginxConfig &config) {}

bool EchoRequestHandler::handle_request(http::request<http::string_body> *http_request, http::response<http::string_body> *http_response) 
{
    std::ostringstream oss;
    oss << *http_request;
    // std::cerr << oss.str() << std::endl;
    http_response->result(http::status::ok); 
    http_response->version(http_request->version());
    http_response->body() = oss.str();
    http_response->prepare_payload();
    return true;
}
bool StaticRequestHandler::handle_request(http::request<http::string_body> *http_request, http::response<http::string_body> *http_response) 
{ 
  return true;
}

bool BadRequestHandler::handle_request(http::request<http::string_body> *http_request, http::response<http::string_body> *http_response) 
{
  http_response->result(http::status::bad_request);
  http_response->body() = "";
  http_response->prepare_payload();
  return false;
}

bool NotFoundRequestHandler::handle_request(http::request<http::string_body> *http_request, http::response<http::string_body> *http_response) 
{ 
  http_response->result(http::status::not_found);
  http_response->body() = "";
  http_response->prepare_payload();
  return false;
}