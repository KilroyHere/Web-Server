#include "gtest/gtest.h"
#include "request_handler_factory.h"
#include "request_handler.h"
#include "request_parser.h"
#include "config_parser.h"
#include "session.h"
#include <fstream>
#include <iostream>
#include <boost/beast/http.hpp>
#include <boost/beast/http/status.hpp>

namespace beast = boost::beast;
namespace http = beast::http;

class RequestParserTest : public ::testing::Test
{
  protected:
    RequestParserTest()
    {
      bool success = parser.Parse("config_parser_tests/new_server_config", &out_config);
      assert(success == true);
    }
    NginxConfigParser parser;
    NginxConfig out_config;
    std::vector<char> data;
    size_t bytes_transferred;
    RequestParser rp;
    Request request_;
};

void extract_from_file(const char *file_name, std::vector<char> &data, size_t &bytes_transferred)
{
  std::ifstream request_file(file_name);
  char c;
  bytes_transferred = 0;
  if (request_file.is_open())
  {
    while (request_file.good())
    {
      request_file.get(c);
      data.push_back(c);
      if (c == '\n')
      {
        data.insert(data.end() - 1, '\r');
      }
    }
  }
  bytes_transferred = data.size();
  request_file.close();
}

TEST_F(RequestParserTest, good_parse)
{
    extract_from_file("request_handler_tests/good_request", data, bytes_transferred);
    RequestParser::result_type result = rp.parse(request_, data, bytes_transferred);
    EXPECT_TRUE(result == RequestParser::result_type::good);
}

TEST_F(RequestParserTest, bad_parse)
{
    extract_from_file("request_handler_tests/bad_http_request_1", data, bytes_transferred);
    RequestParser::result_type result = rp.parse(request_, data, bytes_transferred);
    EXPECT_TRUE(result == RequestParser::result_type::bad);
}

TEST_F(RequestParserTest, indeterminate_parse)
{
    extract_from_file("request_handler_tests/invalid_file", data, bytes_transferred);
    RequestParser::result_type result = rp.parse(request_, data, bytes_transferred);
    EXPECT_TRUE(result == RequestParser::result_type::indeterminate);
}