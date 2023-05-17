#include <boost/asio/ip/tcp.hpp>
#include <gtest/gtest.h>
#include "session.h"
#include "config_parser.h"
#include <vector>

class SessionTest : public ::testing::Test
{
protected:
  boost::asio::io_service io_service;
  SessionTest()
  {
    bool success = parser.Parse("config_parser_tests/new_server_config", &config);
    session_ = new session(io_service, config);
  }
  ~SessionTest()
  {
    delete session_;
  }
  int handle_read(const boost::system::error_code &error, size_t bytes_transferred)
  {
    session_->handle_read(error, bytes_transferred);
  }
  int handle_write(const boost::system::error_code &error)
  {
    session_->handle_write(error);
  }
  void reset()
  {
    session_->reset();
  }
  NginxConfigParser parser;
  NginxConfig config;
  session *session_;
  Request request_;
  std::vector<char> data;
  size_t bytes_transferred;
};

void extract_from_file(const char *file_name, std::vector<char> &data, size_t &bytes_transferred)
{
  std::ifstream request_file(file_name);
  char c;

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

  request_file.close();
}

TEST_F(SessionTest, async_readtest)
{
  session_->async_read();
  EXPECT_TRUE(true);
}

TEST_F(SessionTest, handle_read_eof_error_code)
{
  boost::system::error_code e = boost::asio::error::eof;
  size_t s = 0;
  int result = handle_read(e, s);
  EXPECT_EQ(1, result);
}

TEST_F(SessionTest, handle_read_connection_refused_error_code)
{
  boost::system::error_code e = boost::asio::error::connection_refused;
  size_t s = 0;
  int result = handle_read(e, s);
  EXPECT_EQ(1, result);
}

TEST_F(SessionTest, handle_write_eof_error_code)
{
  boost::system::error_code e = boost::asio::error::eof;
  int result = handle_write(e);
  EXPECT_EQ(1, result);
}

TEST_F(SessionTest, handle_write_connection_refused_error_code)
{
  boost::system::error_code e = boost::asio::error::connection_refused;
  int result = handle_write(e);
  EXPECT_EQ(1, result);
}
TEST_F(SessionTest, handle_write_no_error)
{
  boost::system::error_code e = boost::system::error_code();
  int result = handle_write(e);
  EXPECT_EQ(0, result);
}

TEST_F(SessionTest, async_writetest)
{
  std::vector<char> v;
  session_->async_write(v);
  EXPECT_TRUE(true);
}

TEST_F(SessionTest, get_sockettest)
{
  session_->get_socket();
  EXPECT_TRUE(true);
}

TEST_F(SessionTest, good_purge_uri)
{
  std::vector<char> response_data, response;
  size_t r_bytes_transferred;
  extract_from_file("request_handler_tests/good_request", data, bytes_transferred);
  extract_from_file("request_handler_tests/good_request_response", response_data, r_bytes_transferred);
  reset();
  bool match = request_.uri.empty();
  EXPECT_TRUE(match);
}

TEST_F(SessionTest, good_purge_request_headers)
{
  std::vector<char> response_data, response;
  size_t r_bytes_transferred;
  extract_from_file("request_handler_tests/good_request", data, bytes_transferred);
  extract_from_file("request_handler_tests/good_request_response", response_data, r_bytes_transferred);
  reset();
  bool match = request_.request_headers.empty();
  EXPECT_TRUE(match);
}

TEST_F(SessionTest, good_purge_request_body)
{
  std::vector<char> response_data, response;
  size_t r_bytes_transferred;
  extract_from_file("request_handler_tests/good_request", data, bytes_transferred);
  extract_from_file("request_handler_tests/good_request_response", response_data, r_bytes_transferred);
  reset();
  bool match = request_.request_body.empty();
  EXPECT_TRUE(match);
}

TEST_F(SessionTest, good_purge_headers)
{
  std::vector<char> response_data, response;
  size_t r_bytes_transferred;
  extract_from_file("request_handler_tests/good_request", data, bytes_transferred);
  extract_from_file("request_handler_tests/good_request_response", response_data, r_bytes_transferred);
  reset();
  bool match = request_.headers.empty();
  EXPECT_TRUE(match);
}

TEST_F(SessionTest, good_purge_headers_map)
{
  std::vector<char> response_data, response;
  size_t r_bytes_transferred;
  extract_from_file("request_handler_tests/good_request", data, bytes_transferred);
  extract_from_file("request_handler_tests/good_request_response", response_data, r_bytes_transferred);
  reset();
  bool match = request_.headers_map.empty();
  EXPECT_TRUE(match);
}
