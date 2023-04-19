#include "gtest/gtest.h"
#include "request_handler.h"
#include <fstream>

class RequestHandlerTest : public ::testing::Test
{
protected:
  RequestHandler handler;
  std::vector<char> data;
  size_t bytes_transferred;
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

TEST_F(RequestHandlerTest, normal_request)
{
  extract_from_file("request_handler_tests/good_request", data, bytes_transferred);
  int status = handler.handle_request(data, bytes_transferred);
  EXPECT_EQ(status, 1);
}

TEST_F(RequestHandlerTest, indeterminate_request)
{
  extract_from_file("request_handler_tests/indeterminate_request", data, bytes_transferred);
  int status = handler.handle_request(data, bytes_transferred);
  EXPECT_EQ(status, 0);
}

TEST_F(RequestHandlerTest, bad_method_start_request)
{
  extract_from_file("request_handler_tests/bad_method_start_request", data, bytes_transferred);
  int status = handler.handle_request(data, bytes_transferred);
  EXPECT_EQ(status, 2);
}

TEST_F(RequestHandlerTest, bad_method_request)
{
  extract_from_file("request_handler_tests/bad_method_request", data, bytes_transferred);
  int status = handler.handle_request(data, bytes_transferred);
  EXPECT_EQ(status, 2);
}

TEST_F(RequestHandlerTest, bad_http_request_1)
{
  extract_from_file("request_handler_tests/bad_http_request_1", data, bytes_transferred);
  int status = handler.handle_request(data, bytes_transferred);
  EXPECT_EQ(status, 2);
}

TEST_F(RequestHandlerTest, bad_http_request_2)
{
  extract_from_file("request_handler_tests/bad_http_request_2", data, bytes_transferred);
  int status = handler.handle_request(data, bytes_transferred);
  EXPECT_EQ(status, 2);
}

TEST_F(RequestHandlerTest, bad_http_request_3)
{
  extract_from_file("request_handler_tests/bad_http_request_3", data, bytes_transferred);
  int status = handler.handle_request(data, bytes_transferred);
  EXPECT_EQ(status, 2);
}