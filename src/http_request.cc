#include "http_request.h"

void Request::purge_request()
{
  method.clear();
  uri.clear();
  request_headers.clear();
  request_body.clear();
  headers.clear();
  headers.clear();
  headers_map.clear();
}

void Request::set_headers_map()
{
  for (auto x : headers)
  {
    headers_map[boost::algorithm::to_lower_copy(x.name)] = boost::algorithm::to_lower_copy(x.value);
  }
}
