//
// mime_types.c
// ~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "mime_types.h"

struct mapping
{
  const char *extension;
  const char *mime_type;
} mappings[] =
    {
        {"gif", "image/gif"},
        {"htm", "text/html"},
        {"html", "text/html"},
        {"css", "text/css"},
        {"js", "text/javascript"},
        {"jpg", "image/jpeg"},
        {"png", "image/png"},
        {"pdf", "application/pdf"},
        {"zip", "application/zip"},
        {"txt", "text/plain"}};

std::string extension_to_type(const std::string &extension)
{
  for (mapping m : mappings)
  {
    if (m.extension == extension)
    {
      return m.mime_type;
    }
  }

  return "text/plain";
}
