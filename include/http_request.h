// Adapted from:
// request.hpp
// ~~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <boost/algorithm/string.hpp>
#include <string>
#include <vector>
#include <map>

struct request_header
{
  std::string name;
  std::string value;
};

/// A request received from a client.
class Request
{
public:
  
  std::string method;
  std::string uri;
  std::string path;
  int http_version_major;
  int http_version_minor;
  std::string request_headers;
  std::string request_body;
  std::vector<request_header> headers;
  std::map<std::string, std::string> headers_map;



  void purge_request();
  void set_headers_map();
};

#endif // HTTP_REQUEST_H