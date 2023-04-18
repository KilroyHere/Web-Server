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
  bool handle_request(std::vector<char>, size_t bytes_transferred);
  bool connection_close();
  void set_response();
  std::vector<char> get_response();
  void reset();

private:
  bool read_body(std::vector<char> data, int read_from, int bytes_transferred);

  enum handler_state
  {
    READING_HEADER,
    READING_BODY
  } state_;

  RequestParser parser_;
  Request request_;
  Response response_;
  bool connection_close_;
  // The length of the body read
  int body_read_;
  int response_code_;
};

#endif // REQUEST_HANDLER_H