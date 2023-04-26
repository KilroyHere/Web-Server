#include "gtest/gtest.h"
#include "request_handler.h"
#include "http_request.h"
#include <fstream>

class RequestHandlerTest : public ::testing::Test
{
protected:
  RequestHandler handler;
  Request request_;
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

TEST_F(RequestHandlerTest, good_request)
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

TEST_F(RequestHandlerTest, bad_http_request_4)
{
  extract_from_file("request_handler_tests/bad_http_request_4", data, bytes_transferred);
  int status = handler.handle_request(data, bytes_transferred);
  EXPECT_EQ(status, 2);
}

TEST_F(RequestHandlerTest, bad_http_request_5)
{
  extract_from_file("request_handler_tests/bad_http_request_5", data, bytes_transferred);
  int status = handler.handle_request(data, bytes_transferred);
  EXPECT_EQ(status, 2);
}

TEST_F(RequestHandlerTest, bad_http_request_6)
{
  extract_from_file("request_handler_tests/good_request", data, bytes_transferred);
  data.insert(data.begin() + 5, char(0));
  int status = handler.handle_request(data, bytes_transferred);
  EXPECT_EQ(status, 2);
}

TEST_F(RequestHandlerTest, bad_http_version_request_1)
{
  extract_from_file("request_handler_tests/bad_http_version_request_1", data, bytes_transferred);
  int status = handler.handle_request(data, bytes_transferred);
  EXPECT_EQ(status, 2);
}

TEST_F(RequestHandlerTest, bad_http_version_request_2)
{
  extract_from_file("request_handler_tests/bad_http_version_request_2", data, bytes_transferred);
  int status = handler.handle_request(data, bytes_transferred);
  EXPECT_EQ(status, 2);
}

TEST_F(RequestHandlerTest, bad_http_version_request_3)
{
  extract_from_file("request_handler_tests/bad_http_version_request_3", data, bytes_transferred);
  int status = handler.handle_request(data, bytes_transferred);
  EXPECT_EQ(status, 2);
}

TEST_F(RequestHandlerTest, bad_http_version_request_4)
{
  extract_from_file("request_handler_tests/bad_http_version_request_4", data, bytes_transferred);
  int status = handler.handle_request(data, bytes_transferred);
  EXPECT_EQ(status, 2);
}

TEST_F(RequestHandlerTest, good_http_version_request_1)
{
  extract_from_file("request_handler_tests/good_http_version_request_1", data, bytes_transferred);
  int status = handler.handle_request(data, bytes_transferred);
  EXPECT_EQ(status, 1);
}

TEST_F(RequestHandlerTest, good_http_version_request_2)
{
  extract_from_file("request_handler_tests/good_http_version_request_2", data, bytes_transferred);
  int status = handler.handle_request(data, bytes_transferred);
  EXPECT_EQ(status, 1);
}

TEST_F(RequestHandlerTest, bad_header_request_1)
{
  extract_from_file("request_handler_tests/bad_header_request_1", data, bytes_transferred);
  int status = handler.handle_request(data, bytes_transferred);
  EXPECT_EQ(status, 2);
}

TEST_F(RequestHandlerTest, bad_header_request_2)
{
  extract_from_file("request_handler_tests/bad_header_request_2", data, bytes_transferred);
  int status = handler.handle_request(data, bytes_transferred);
  EXPECT_EQ(status, 2);
}

TEST_F(RequestHandlerTest, bad_header_request_3)
{
  extract_from_file("request_handler_tests/bad_header_request_3", data, bytes_transferred);
  int status = handler.handle_request(data, bytes_transferred);
  EXPECT_EQ(status, 2);
}

TEST_F(RequestHandlerTest, bad_header_request_4)
{
  extract_from_file("request_handler_tests/good_request", data, bytes_transferred);
  data.insert(data.begin() + 34, char(0));
  int status = handler.handle_request(data, bytes_transferred);
  EXPECT_EQ(status, 2);
}

TEST_F(RequestHandlerTest, bad_new_line_request_1)
{
  extract_from_file("request_handler_tests/good_request", data, bytes_transferred);
  data.insert(data.begin() + 15, '0');
  int status = handler.handle_request(data, bytes_transferred);
  EXPECT_EQ(status, 2);
}

TEST_F(RequestHandlerTest, bad_new_line_request_2)
{
  extract_from_file("request_handler_tests/good_request", data, bytes_transferred);
  data.insert(data.begin() + 35, '0');
  int status = handler.handle_request(data, bytes_transferred);
  EXPECT_EQ(status, 2);
}

TEST_F(RequestHandlerTest, good_lws_request_1)
{
  extract_from_file("request_handler_tests/good_lws_request_1", data, bytes_transferred);
  int status = handler.handle_request(data, bytes_transferred);
  EXPECT_EQ(status, 1);
}

TEST_F(RequestHandlerTest, good_lws_request_2)
{
  extract_from_file("request_handler_tests/good_lws_request_2", data, bytes_transferred);
  int status = handler.handle_request(data, bytes_transferred);
  EXPECT_EQ(status, 1);
}

TEST_F(RequestHandlerTest, good_lws_request_3)
{
  extract_from_file("request_handler_tests/good_lws_request_3", data, bytes_transferred);
  int status = handler.handle_request(data, bytes_transferred);
  EXPECT_EQ(status, 1);
}

TEST_F(RequestHandlerTest, bad_lws_request)
{
  extract_from_file("request_handler_tests/good_lws_request_1", data, bytes_transferred);
  data.insert(data.begin() + 37, char(0));
  int status = handler.handle_request(data, bytes_transferred);
  EXPECT_EQ(status, 2);
}

TEST_F(RequestHandlerTest, good_request_with_response)
{
  std::vector<char> response_data, response;
  size_t r_bytes_transferred;
  extract_from_file("request_handler_tests/good_request", data, bytes_transferred);
  extract_from_file("request_handler_tests/good_request_response", response_data, r_bytes_transferred);
  int status = handler.handle_request(data, bytes_transferred);
  handler.set_response();
  response = handler.get_response();
  bool match = response == response_data;
  EXPECT_EQ(status, 1);
  EXPECT_TRUE(match);
}

TEST_F(RequestHandlerTest, bad_request_with_response)
{
  std::vector<char> response_data, response;
  size_t r_bytes_transferred;
  extract_from_file("request_handler_tests/bad_http_request_1", data, bytes_transferred);
  extract_from_file("request_handler_tests/bad_request_response", response_data, r_bytes_transferred);
  int status = handler.handle_request(data, bytes_transferred);
  handler.set_response();
  response = handler.get_response();
  bool match = response == response_data;
  EXPECT_EQ(status, 2);
  EXPECT_TRUE(match);
}

TEST_F(RequestHandlerTest, good_purge_request_uri)
{
  std::vector<char> response_data, response;
  size_t r_bytes_transferred;
  extract_from_file("request_handler_tests/good_request", data, bytes_transferred);
  extract_from_file("request_handler_tests/good_request_response", response_data, r_bytes_transferred);
  handler.reset();
  bool match = request_.uri.empty();
  EXPECT_TRUE(match);
}

TEST_F(RequestHandlerTest, good_purge_request_headers)
{
  std::vector<char> response_data, response;
  size_t r_bytes_transferred;
  extract_from_file("request_handler_tests/good_request", data, bytes_transferred);
  extract_from_file("request_handler_tests/good_request_response", response_data, r_bytes_transferred);
  handler.reset();
  bool match = request_.request_headers.empty();
  EXPECT_TRUE(match);
}

TEST_F(RequestHandlerTest, good_purge_request_body)
{
  std::vector<char> response_data, response;
  size_t r_bytes_transferred;
  extract_from_file("request_handler_tests/good_request", data, bytes_transferred);
  extract_from_file("request_handler_tests/good_request_response", response_data, r_bytes_transferred);
  handler.reset();
  bool match = request_.request_body.empty();
  EXPECT_TRUE(match);
}

TEST_F(RequestHandlerTest, good_purge_headers)
{
  std::vector<char> response_data, response;
  size_t r_bytes_transferred;
  extract_from_file("request_handler_tests/good_request", data, bytes_transferred);
  extract_from_file("request_handler_tests/good_request_response", response_data, r_bytes_transferred);
  handler.reset();
  bool match = request_.headers.empty();
  EXPECT_TRUE(match);
}

TEST_F(RequestHandlerTest, good_purge_headers_map)
{
  std::vector<char> response_data, response;
  size_t r_bytes_transferred;
  extract_from_file("request_handler_tests/good_request", data, bytes_transferred);
  extract_from_file("request_handler_tests/good_request_response", response_data, r_bytes_transferred);
  handler.reset();
  bool match = request_.headers_map.empty();
  EXPECT_TRUE(match);
}

TEST_F(RequestHandlerTest, good_request_with_close_connection)
{
  extract_from_file("request_handler_tests/good_request_with_close_connection", data, bytes_transferred);
  int status = handler.handle_request(data, bytes_transferred);
  int connection_status = handler.connection_close();
  EXPECT_EQ(status, 1);
  EXPECT_EQ(connection_status, 1);
}

TEST_F(RequestHandlerTest, good_request_with_open_connection)
{
  extract_from_file("request_handler_tests/good_request_with_open_connection", data, bytes_transferred);
  int status = handler.handle_request(data, bytes_transferred);
  int connection_status = handler.connection_close();
  EXPECT_EQ(status, 1);
  EXPECT_EQ(connection_status, 0);
}

TEST_F(RequestHandlerTest, good_request_with_good_body)
{
  extract_from_file("request_handler_tests/good_request_with_good_body", data, bytes_transferred);
  int status = handler.handle_request(data, bytes_transferred);
  EXPECT_EQ(status, 1);
}

TEST_F(RequestHandlerTest, good_request_with_bad_body)
{
  extract_from_file("request_handler_tests/good_request_with_bad_body", data, bytes_transferred);
  int status = handler.handle_request(data, bytes_transferred);
  EXPECT_EQ(status, 0);
}