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

std::vector<char> Response::to_buffer()
{
  std::vector<char> buffer;
  std::string response = "";
  response += status_to_string(status_code_);
  for (std::size_t i = 0; i < headers_.size(); ++i)
  {
    response_header &h = headers_[i];
    response += h.name;
    response += name_value_separator;
    response += h.value;
    response += crlf;
  }
  response += crlf;
  // LOGGING:
  std::cerr<<"=====RESPONSE=====\n";
  for (auto c : response)
  {
    // LOGGING:
    std::cerr << c;
    
  }
  // LOGGING:

  std::cerr<<"\n==================\n";

  response += content_;
  for (auto c : response)
  {    
    
    buffer.push_back(c);
  }
  
  return buffer;
}

std::string Response::status_to_string(int status)
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
void Response::set_echo_response(int status, const std::string response_body)
{
  status_code_ = status;
  content_ = response_body;
  headers_.resize(2);
  headers_[0].name = "Content-Length";
  headers_[0].value = std::to_string(int(content_.size()));
  headers_[1].name = "Content-Type";
  headers_[1].value = "text/plain";
}


void Response::set_file_response(int status, const std::string filepath) 
{
  
  //read file contents 
  std::ifstream file(filepath.c_str(), std::ios::in | std::ios::binary);

  std::cerr<<"Reading the file "<<"\n";
  std::string body; 
  
  char c;
  while (file.get(c)) body += c;

  std::cerr<< "Got file contents"<<"\n";
  
  file.close();

  std::cerr<<"File closed"<<"\n";

  //map mime type 

  //get extension of the file to map to mime 
  std::string extension;
  size_t cursor = filepath.find_last_of(".");
  if (cursor != std::string::npos)
    extension = filepath.substr(cursor + 1);
  
  status_code_ = status; 
  content_ = body; 
  
  headers_.resize(2);
  headers_[0].name = "Content-Length";
  headers_[0].value = std::to_string(content_.length());
  headers_[1].name = "Content-Type";
  headers_[1].value = extension_to_type(extension);

}

void Response::purge_response()
{
  content_.clear();
  headers_.clear();
}