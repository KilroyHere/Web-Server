// Adapted from:
// response.cpp
// ~~~~~~~~~
//
// Copyright (c) 2003-2020 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.1. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "http_response.h"
#include <cstdlib>

std::string name_value_separator = ": ";
std::string crlf = "\r\n";

std::vector<char> response::to_buffer()
{
  std::vector<char> buffer;
  std::string response = "";
  response += status_to_string(status_);
  for (std::size_t i = 0; i < headers_.size(); ++i)
  {
    response_header &h = headers_[i];
    response += h.name;
    response += name_value_separator;
    response += h.value;
    response += crlf;
  }
  response += crlf;
  response += content_;

  for (auto c : response)
  {
    std::cerr << c;
    buffer.push_back(c);
  }
  return buffer;
}

std::string response::status_to_string(int status)
{
  if(status_map_.find(status) != status_map_.end())
  {
    return status_map_[status];
  }
  else
  {
    // Error
    std::cerr << "Wrong status code";
    return status_map_[400];
  }
}

// For Echo server specifically
void response::set_echo_response(int status, const std::string response_body)
{
  status_ = status;
  content_ = response_body;
  headers_.resize(2);
  headers_[0].name = "Content-Length";
  headers_[0].value = std::to_string(int(content_.size()));
  headers_[1].name = "Content-Type";
  headers_[1].value = "text/plain";
}

void response::purge_response()
{
  content_.clear();
  headers_.clear();
}