#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <sys/stat.h>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include "request_parser.h"
#include "http_request.h"
#include "http_response.h"
#include "config_parser.h"
#include "response_handler.h"
#include "mime_types.h"

class RequestHandler
{
public:
  // TODO
  // RequestHandler();
  RequestHandler(NginxConfig config);
  // Handle the read data to parse and respond to a request
  // Return Value: 0 for indeterminate, 1 for good request, 2 for bad request
  int handle_request(std::vector<char> data, size_t bytes_transferred);
  // Get the value of the connection_close_ flag
  bool connection_close();
  // Set the response according to the response_code_
  void set_response();
  // Get the response in a vector<char> format
  std::vector<char> get_response();
  // Reset the handler to handler another request
  void reset();

private:
  // Read the body content of the request/
  // Return Value: 1 for read complete, 0 for read incomplete
  int read_body(std::vector<char> data, int read_from, int bytes_transferred);
  // The state of the request_handler
  void set_path_root_map();
  enum handler_state
  {
    READING_HEADER,
    READING_BODY
  } state_;

  NginxConfig config_;
  RequestParser parser_;
  Request request_;
  Response response_;

  std::unordered_map<std::string, std::string> path_root_map_;
  // Flag that determines if a connection is to be closed
  bool connection_close_;
  // The length of the body read
  int body_read_;
  // The response code to be set according to the request
  int response_code_;
};

#endif // REQUEST_HANDLER_H