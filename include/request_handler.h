#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <iostream>
#include <vector>
#include "request_parser.h"
#include "http_request.h"
#include "http_response.h"

class RequestHandler
{
public:
  RequestHandler();
  // Handle the read data to parse and respond to a request 
  bool handle_request(std::vector<char> data, size_t bytes_transferred);
  // Get the value of the connection_close_ flag
  bool connection_close();
  // Set the response according to the response_code_
  void set_response();
  // Get the response in a vector<char> format
  std::vector<char> get_response();
  // Reset the handler to handler another request
  void reset();

private:
  // Read the body content of the request
  bool read_body(std::vector<char> data, int read_from, int bytes_transferred);
  // The state of the request_handler
  enum handler_state
  {
    READING_HEADER,
    READING_BODY
  } state_;

  RequestParser parser_;
  Request request_;
  Response response_;
  // Flag that determines if a connection is to be closed
  bool connection_close_;
  // The length of the body read
  int body_read_;
  // The response code to be set according to the request
  int response_code_;
};

#endif // REQUEST_HANDLER_H