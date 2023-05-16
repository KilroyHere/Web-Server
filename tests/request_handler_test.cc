#include "gtest/gtest.h"
#include "request_handler_factory.h"
#include "request_handler.h"
#include "config_parser.h"
#include <fstream>
#include <iostream>
#include <boost/beast/http.hpp>
#include <boost/beast/http/status.hpp>

namespace beast = boost::beast;
namespace http = beast::http;

class RequestHandlerTest : public ::testing::Test
{
  protected:
    RequestHandlerTest()
    {
      bool success = parser.Parse("config_parser_tests/new_server_config", &out_config); // TODO: change new_server_config -> server_config and delete old one.
      assert(success == true);
    }
    NginxConfigParser parser;
    NginxConfig out_config;
};


// class RequestHandlerTest : public ::testing::Test
// {
// protected:
//   NginxConfigParser parser;
//   NginxConfig out_config; 
//   Request request_;
//   std::vector<char> data;
//   size_t bytes_transferred;
//   RequestHandler* handler= nullptr;
//   RequestHandlerTest() {
//     bool success = parser.Parse("config_parser_tests/deploy_config", &out_config);
//     handler = new RequestHandler(out_config);
//   }
//   ~RequestHandlerTest() {
//     delete handler;
//   }
// };

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

TEST_F(RequestHandlerTest, default_404_request)
{
  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req{http::verb::get, "/", 10};
  std::unique_ptr<RequestHandler> handler = nhf.createHandler(&req);
  http::response<http::string_body> res;
  handler->handle_request(req, &res);
  EXPECT_TRUE(res.body() == "");
  EXPECT_TRUE(res.result() == http::status::not_found);
}

TEST_F(RequestHandlerTest, good_echo_request)
{
  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req{http::verb::get, "/echo", 10};
  std::unique_ptr<RequestHandler> handler = nhf.createHandler(&req);
  http::response<http::string_body> res;
  handler->handle_request(req, &res);
  std::ostringstream oss;
  oss << req;
  EXPECT_TRUE(res.body() == oss.str());
  EXPECT_TRUE(res.result() == http::status::ok);
}

TEST_F(RequestHandlerTest, good_static_index_html_request)
{
  size_t bytes_transferred;
  std::vector<char> response_data;
  std::string file_path = "./request_handler_tests/good_request_with_response_index_html";
  std::ifstream file(file_path.c_str(), std::ios::in | std::ios::binary);
  std::string body = "";

  char c;
  while (file.get(c))
    body += c;

  file.close();

  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req{http::verb::get, "/static/index.html", 10};
  std::unique_ptr<RequestHandler> handler = nhf.createHandler(&req);
  http::response<http::string_body> res;
  handler->handle_request(req, &res);
  EXPECT_TRUE(res.body() == body);
  EXPECT_TRUE(res.result() == http::status::ok);
}

TEST_F(RequestHandlerTest, good_static_mime_jpeg_test_request)
{
  size_t bytes_transferred;
  std::vector<char> response_data;
  std::string file_path = "./request_handler_tests/good_request_with_response_mime_jpeg_test";
  std::ifstream file(file_path.c_str(), std::ios::in | std::ios::binary);
  std::string body = "";

  char c;
  while (file.get(c))
    body += c;

  file.close();

  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req{http::verb::get, "/static/mime_jpeg_test.jpeg", 10};
  std::unique_ptr<RequestHandler> handler = nhf.createHandler(&req);
  http::response<http::string_body> res;
  handler->handle_request(req, &res);
  EXPECT_TRUE(res.body() == body);
  EXPECT_TRUE(res.result() == http::status::ok);
}

// TEST_F(RequestHandlerTest, good_request)
// {
//   extract_from_file("request_handler_tests/good_request", data, bytes_transferred);
//   int status = handler->handle_request(data, bytes_transferred);
//   EXPECT_EQ(status, 1);
// }

// TEST_F(RequestHandlerTest, indeterminate_request)
// {
//   extract_from_file("request_handler_tests/indeterminate_request", data, bytes_transferred);
//   int status = handler->handle_request(data, bytes_transferred);
//   EXPECT_EQ(status, 0);
// }

// TEST_F(RequestHandlerTest, bad_method_start_request)
// {
//   extract_from_file("request_handler_tests/bad_method_start_request", data, bytes_transferred);
//   int status = handler->handle_request(data, bytes_transferred);
//   EXPECT_EQ(status, 2);
// }

// TEST_F(RequestHandlerTest, bad_method_request)
// {
//   extract_from_file("request_handler_tests/bad_method_request", data, bytes_transferred);
//   int status = handler->handle_request(data, bytes_transferred);
//   EXPECT_EQ(status, 2);
// }

// TEST_F(RequestHandlerTest, bad_http_request_1)
// {
//   extract_from_file("request_handler_tests/bad_http_request_1", data, bytes_transferred);
//   int status = handler->handle_request(data, bytes_transferred);
//   EXPECT_EQ(status, 2);
// }

// TEST_F(RequestHandlerTest, bad_http_request_2)
// {
//   extract_from_file("request_handler_tests/bad_http_request_2", data, bytes_transferred);
//   int status = handler->handle_request(data, bytes_transferred);
//   EXPECT_EQ(status, 2);
// }

// TEST_F(RequestHandlerTest, bad_http_request_3)
// {
//   extract_from_file("request_handler_tests/bad_http_request_3", data, bytes_transferred);
//   int status = handler->handle_request(data, bytes_transferred);
//   EXPECT_EQ(status, 2);
// }

// TEST_F(RequestHandlerTest, bad_http_request_4)
// {
//   extract_from_file("request_handler_tests/bad_http_request_4", data, bytes_transferred);
//   int status = handler->handle_request(data, bytes_transferred);
//   EXPECT_EQ(status, 2);
// }

// TEST_F(RequestHandlerTest, bad_http_request_5)
// {
//   extract_from_file("request_handler_tests/bad_http_request_5", data, bytes_transferred);
//   int status = handler->handle_request(data, bytes_transferred);
//   EXPECT_EQ(status, 2);
// }

// TEST_F(RequestHandlerTest, bad_http_request_6)
// {
//   extract_from_file("request_handler_tests/good_request", data, bytes_transferred);
//   data.insert(data.begin() + 5, char(0));
//   int status = handler->handle_request(data, bytes_transferred);
//   EXPECT_EQ(status, 2);
// }

// TEST_F(RequestHandlerTest, bad_http_version_request_1)
// {
//   extract_from_file("request_handler_tests/bad_http_version_request_1", data, bytes_transferred);
//   int status = handler->handle_request(data, bytes_transferred);
//   EXPECT_EQ(status, 2);
// }

// TEST_F(RequestHandlerTest, bad_http_version_request_2)
// {
//   extract_from_file("request_handler_tests/bad_http_version_request_2", data, bytes_transferred);
//   int status = handler->handle_request(data, bytes_transferred);
//   EXPECT_EQ(status, 2);
// }

// TEST_F(RequestHandlerTest, bad_http_version_request_3)
// {
//   extract_from_file("request_handler_tests/bad_http_version_request_3", data, bytes_transferred);
//   int status = handler->handle_request(data, bytes_transferred);
//   EXPECT_EQ(status, 2);
// }

// TEST_F(RequestHandlerTest, bad_http_version_request_4)
// {
//   extract_from_file("request_handler_tests/bad_http_version_request_4", data, bytes_transferred);
//   int status = handler->handle_request(data, bytes_transferred);
//   EXPECT_EQ(status, 2);
// }

// TEST_F(RequestHandlerTest, good_http_version_request_1)
// {
//   extract_from_file("request_handler_tests/good_http_version_request_1", data, bytes_transferred);
//   int status = handler->handle_request(data, bytes_transferred);
//   EXPECT_EQ(status, 1);
// }

// TEST_F(RequestHandlerTest, good_http_version_request_2)
// {
//   extract_from_file("request_handler_tests/good_http_version_request_2", data, bytes_transferred);
//   int status = handler->handle_request(data, bytes_transferred);
//   EXPECT_EQ(status, 1);
// }

// TEST_F(RequestHandlerTest, bad_header_request_1)
// {
//   extract_from_file("request_handler_tests/bad_header_request_1", data, bytes_transferred);
//   int status = handler->handle_request(data, bytes_transferred);
//   EXPECT_EQ(status, 2);
// }

// TEST_F(RequestHandlerTest, bad_header_request_2)
// {
//   extract_from_file("request_handler_tests/bad_header_request_2", data, bytes_transferred);
//   int status = handler->handle_request(data, bytes_transferred);
//   EXPECT_EQ(status, 2);
// }

// TEST_F(RequestHandlerTest, bad_header_request_3)
// {
//   extract_from_file("request_handler_tests/bad_header_request_3", data, bytes_transferred);
//   int status = handler->handle_request(data, bytes_transferred);
//   EXPECT_EQ(status, 2);
// }

// TEST_F(RequestHandlerTest, bad_header_request_4)
// {
//   extract_from_file("request_handler_tests/good_request", data, bytes_transferred);
//   data.insert(data.begin() + 34, char(0));
//   int status = handler->handle_request(data, bytes_transferred);
//   EXPECT_EQ(status, 2);
// }

// TEST_F(RequestHandlerTest, bad_new_line_request_1)
// {
//   extract_from_file("request_handler_tests/good_request", data, bytes_transferred);
//   data.insert(data.begin() + 15, '0');
//   int status = handler->handle_request(data, bytes_transferred);
//   EXPECT_EQ(status, 0);
// }

// TEST_F(RequestHandlerTest, bad_new_line_request_2)
// {
//   extract_from_file("request_handler_tests/good_request", data, bytes_transferred);
//   data.insert(data.begin() + 35, '0');
//   int status = handler->handle_request(data, bytes_transferred);
//   EXPECT_EQ(status, 0);
// }

// TEST_F(RequestHandlerTest, good_lws_request_1)
// {
//   extract_from_file("request_handler_tests/good_lws_request_1", data, bytes_transferred);
//   int status = handler->handle_request(data, bytes_transferred);
//   EXPECT_EQ(status, 1);
// }

// TEST_F(RequestHandlerTest, good_lws_request_2)
// {
//   extract_from_file("request_handler_tests/good_lws_request_2", data, bytes_transferred);
//   int status = handler->handle_request(data, bytes_transferred);
//   EXPECT_EQ(status, 1);
// }

// TEST_F(RequestHandlerTest, good_lws_request_3)
// {
//   extract_from_file("request_handler_tests/good_lws_request_3", data, bytes_transferred);
//   int status = handler->handle_request(data, bytes_transferred);
//   EXPECT_EQ(status, 1);
// }

// TEST_F(RequestHandlerTest, bad_lws_request)
// {
//   extract_from_file("request_handler_tests/good_lws_request_1", data, bytes_transferred);
//   data.insert(data.begin() + 37, char(0));
//   int status = handler->handle_request(data, bytes_transferred);
//   EXPECT_EQ(status, 2);
// }

// TEST_F(RequestHandlerTest, good_request_with_response)
// {
//   std::vector<char> response_data, response;
//   size_t r_bytes_transferred;
//   extract_from_file("request_handler_tests/good_request", data, bytes_transferred);
//   extract_from_file("request_handler_tests/good_request_response", response_data, r_bytes_transferred);
//   int status = handler->handle_request(data, bytes_transferred);
//   handler->set_response();
//   response = handler->get_response();
//   bool match = response == response_data;
//   EXPECT_EQ(status, 1);
//   EXPECT_TRUE(match);
// }

// TEST_F(RequestHandlerTest, bad_request_with_response)
// {
//   std::vector<char> response_data, response;
//   size_t r_bytes_transferred;
//   extract_from_file("request_handler_tests/bad_http_request_1", data, bytes_transferred);
//   extract_from_file("request_handler_tests/bad_request_response", response_data, r_bytes_transferred);
//   int status = handler->handle_request(data, bytes_transferred);
//   handler->set_response();
//   response = handler->get_response();
//   bool match = response == response_data;
//   EXPECT_EQ(status, 2);
//   EXPECT_TRUE(match);
// }

// TEST_F(RequestHandlerTest, good_purge_request_uri)
// {
//   std::vector<char> response_data, response;
//   size_t r_bytes_transferred;
//   extract_from_file("request_handler_tests/good_request", data, bytes_transferred);
//   extract_from_file("request_handler_tests/good_request_response", response_data, r_bytes_transferred);
//   handler->reset();
//   bool match = request_.uri.empty();
//   EXPECT_TRUE(match);
// }

// TEST_F(RequestHandlerTest, good_purge_request_headers)
// {
//   std::vector<char> response_data, response;
//   size_t r_bytes_transferred;
//   extract_from_file("request_handler_tests/good_request", data, bytes_transferred);
//   extract_from_file("request_handler_tests/good_request_response", response_data, r_bytes_transferred);
//   handler->reset();
//   bool match = request_.request_headers.empty();
//   EXPECT_TRUE(match);
// }

// TEST_F(RequestHandlerTest, good_purge_request_body)
// {
//   std::vector<char> response_data, response;
//   size_t r_bytes_transferred;
//   extract_from_file("request_handler_tests/good_request", data, bytes_transferred);
//   extract_from_file("request_handler_tests/good_request_response", response_data, r_bytes_transferred);
//   handler->reset();
//   bool match = request_.request_body.empty();
//   EXPECT_TRUE(match);
// }

// TEST_F(RequestHandlerTest, good_purge_headers)
// {
//   std::vector<char> response_data, response;
//   size_t r_bytes_transferred;
//   extract_from_file("request_handler_tests/good_request", data, bytes_transferred);
//   extract_from_file("request_handler_tests/good_request_response", response_data, r_bytes_transferred);
//   handler->reset();
//   bool match = request_.headers.empty();
//   EXPECT_TRUE(match);
// }

// TEST_F(RequestHandlerTest, good_purge_headers_map)
// {
//   std::vector<char> response_data, response;
//   size_t r_bytes_transferred;
//   extract_from_file("request_handler_tests/good_request", data, bytes_transferred);
//   extract_from_file("request_handler_tests/good_request_response", response_data, r_bytes_transferred);
//   handler->reset();
//   bool match = request_.headers_map.empty();
//   EXPECT_TRUE(match);
// }

// TEST_F(RequestHandlerTest, good_request_with_close_connection)
// {
//   extract_from_file("request_handler_tests/good_request_with_close_connection", data, bytes_transferred);
//   int status = handler->handle_request(data, bytes_transferred);
//   int connection_status = handler->connection_close();
//   EXPECT_EQ(status, 1);
//   EXPECT_EQ(connection_status, 1);
// }

// TEST_F(RequestHandlerTest, good_request_with_open_connection)
// {
//   extract_from_file("request_handler_tests/good_request_with_open_connection", data, bytes_transferred);
//   int status = handler->handle_request(data, bytes_transferred);
//   int connection_status = handler->connection_close();
//   EXPECT_EQ(status, 1);
//   EXPECT_EQ(connection_status, 0);
// }

// TEST_F(RequestHandlerTest, good_request_with_good_body)
// {
//   extract_from_file("request_handler_tests/good_request_with_good_body", data, bytes_transferred);
//   int status = handler->handle_request(data, bytes_transferred);
//   EXPECT_EQ(status, 1);
// }

// TEST_F(RequestHandlerTest, good_request_with_bad_body)
// {
//   extract_from_file("request_handler_tests/good_request_with_bad_body", data, bytes_transferred);
//   int status = handler->handle_request(data, bytes_transferred);
//   EXPECT_EQ(status, 0);
// }

// TEST_F(RequestHandlerTest, good_static_request)
// {
//   std::vector<char> response_data, response;
//   size_t r_bytes_transferred;
//   extract_from_file("request_handler_tests/good_static_request", data, bytes_transferred);
//   extract_from_file("request_handler_tests/good_static_request_response", response_data, r_bytes_transferred);
//   int status = handler->handle_request(data, bytes_transferred);
//   handler->set_response();
//   response = handler->get_response();
//   EXPECT_EQ(status, 1);
// }

// TEST_F(RequestHandlerTest, bad_static_request)
// {
//   std::vector<char> response_data, response;
//   size_t r_bytes_transferred;
//   extract_from_file("request_handler_tests/bad_static_request", data, bytes_transferred);
//   extract_from_file("request_handler_tests/bad_static_request_response", response_data, r_bytes_transferred);
//   int status = handler->handle_request(data, bytes_transferred);
//   handler->set_response();
//   response = handler->get_response();
//   bool match = response == response_data;
//   EXPECT_EQ(status, 1);
//   EXPECT_TRUE(match);
// }