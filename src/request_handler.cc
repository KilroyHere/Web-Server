#include "request_handler.h"

RequestHandler::RequestHandler() : state_(READING_HEADER), connection_close_(false), response_code_(200), body_read_(0)
{
}

int RequestHandler::handle_request(std::vector<char> data, size_t bytes_transferred)
{
  // If currenlty reading the header
  if (state_ == READING_HEADER)
  {
    RequestParser::result_type result = parser_.parse(request_, data, bytes_transferred);
    // If read the header successfully and completely
    if (result == RequestParser::good)
    {
      // LOGGING:
      std::cerr << "==================\nGOOD REQUEST!!\n==================\n";
      response_code_ = 200;
      // TODO: Maybe make this a data member?
      int read_from = parser_.read_from_;
      request_.set_headers_map();
      parser_.reset();

      // If connection:close, end connection
      if (request_.headers_map.find("connection") != request_.headers_map.end())
      {
        if (request_.headers_map["connection"] == "close")
        {
          connection_close_ = true;
        }
      }

      //  Check for content-length and parse body
      if (request_.headers_map.find("content-length") != request_.headers_map.end())
      {
        // Switch handler state
        state_ = READING_BODY;
        return read_body(data, read_from, bytes_transferred);
      }
      return 1;
    }
    // If the HTTP request is malformed
    else if (result == RequestParser::bad)
    {
      // LOGGING:
      std::cerr << "==================\nBAD REQUEST!!\n==================\n";
      response_code_ = 400;
      connection_close_ = true;
      return 2;
    }
    // If the header hasn't been read completely
    else
    {
      return 0;
    }
  }
  // If currently reading the body
  else
  {
    int read_from = parser_.read_from_;
    return read_body(data, read_from, bytes_transferred);
  }
}

void RequestHandler::reset()
{
  // Ready for a new request in the same session
  state_ = READING_HEADER;
  body_read_ = 0;
  request_.purge_request();
  response_.purge_response();
}

int RequestHandler::read_body(std::vector<char> data, int read_from, int bytes_transferred)
{
  int content_length = stoi(request_.headers_map["content-length"]);

  for (int i = read_from; i < bytes_transferred && body_read_ < content_length; i++)
  {
    request_.request_body.push_back(data[i]);
    body_read_ += 1;
  }
  if (body_read_ == content_length)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

void RequestHandler::set_response()
{
  // Echo response
  if (response_code_ == 200)
  {
    std::string response_body = request_.request_headers + request_.request_body;
    response_.set_echo_response(200, response_body);
  }
  else
  {
    response_.set_echo_response(400, "");
  }
}

std::vector<char> RequestHandler::get_response()
{
  // Echo response
  return response_.to_buffer();
}

bool RequestHandler::connection_close()
{
  return connection_close_;
}