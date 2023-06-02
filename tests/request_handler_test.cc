#include "gtest/gtest.h"
#include "request_handler_factory.h"
#include "request_handler.h"
#include "config_parser.h"
#include "session.h"
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
    bool success = parser.Parse("config_parser_tests/new_server_config", &out_config);
    assert(success == true);
  }
  void TearDown() override
  {
    boost::filesystem::path dir_to_remove("../folder4");
    boost::filesystem::remove_all(dir_to_remove);
  }
  NginxConfigParser parser;
  NginxConfig out_config;
};

void extract_from_file(const char *file_name, std::vector<char> &data)
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

TEST_F(RequestHandlerTest, bad_echo_request)
{
  std::vector<char> data;
  // extract_from_file("request_handler_tests/bad_header_request_1", data);

  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req{http::verb::get, "/", 1};
  std::unique_ptr<RequestHandler> handler = nhf.createHandler(&req);
  http::response<http::string_body> res;
  handler->handle_request(req, &res);
  EXPECT_TRUE(res.result() == http::status::not_found);
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

TEST_F(RequestHandlerTest, good_static_mime_txt_test_request)
{
  size_t bytes_transferred;
  std::vector<char> response_data;
  std::string file_path = "./request_handler_tests/good_request_with_response_mime_txt_test";
  std::ifstream file(file_path.c_str(), std::ios::in | std::ios::binary);
  std::string body = "";

  char c;
  while (file.get(c))
    body += c;

  file.close();

  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req{http::verb::get, "/static/mime_txt_test.txt", 10};
  std::unique_ptr<RequestHandler> handler = nhf.createHandler(&req);
  http::response<http::string_body> res;
  handler->handle_request(req, &res);
  EXPECT_TRUE(res.body() == body);
  EXPECT_TRUE(res.result() == http::status::ok);
}

TEST_F(RequestHandlerTest, good_static_ziptest_pdf_request)
{
  size_t bytes_transferred;
  std::vector<char> response_data;
  std::string file_path = "./request_handler_tests/good_request_with_response_ziptest_pdf";
  std::ifstream file(file_path.c_str(), std::ios::in | std::ios::binary);
  std::string body = "";

  char c;
  while (file.get(c))
    body += c;

  file.close();

  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req{http::verb::get, "/static/ziptest.pdf", 10};
  std::unique_ptr<RequestHandler> handler = nhf.createHandler(&req);
  http::response<http::string_body> res;
  handler->handle_request(req, &res);
  EXPECT_TRUE(res.body() == body);
  EXPECT_TRUE(res.result() == http::status::ok);
}

TEST_F(RequestHandlerTest, good_static_mime_pdf_test_request)
{
  size_t bytes_transferred;
  std::vector<char> response_data;
  std::string file_path = "./request_handler_tests/good_request_with_response_mime_pdf_test";
  std::ifstream file(file_path.c_str(), std::ios::in | std::ios::binary);
  std::string body = "";

  char c;
  while (file.get(c))
    body += c;

  file.close();

  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req{http::verb::get, "/static1/mime_pdf_test.pdf", 10};
  std::unique_ptr<RequestHandler> handler = nhf.createHandler(&req);
  http::response<http::string_body> res;
  handler->handle_request(req, &res);
  EXPECT_TRUE(res.body() == body);
  EXPECT_TRUE(res.result() == http::status::ok);
}

TEST_F(RequestHandlerTest, good_static_mime_png_test_request)
{
  size_t bytes_transferred;
  std::vector<char> response_data;
  std::string file_path = "./request_handler_tests/good_request_with_response_mime_png_test";
  std::ifstream file(file_path.c_str(), std::ios::in | std::ios::binary);
  std::string body = "";

  char c;
  while (file.get(c))
    body += c;

  file.close();

  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req{http::verb::get, "/static1/mime_png_test.png", 10};
  std::unique_ptr<RequestHandler> handler = nhf.createHandler(&req);
  http::response<http::string_body> res;
  handler->handle_request(req, &res);
  EXPECT_TRUE(res.body() == body);
  EXPECT_TRUE(res.result() == http::status::ok);
}

TEST_F(RequestHandlerTest, good_static_mime_zip_test_request)
{
  size_t bytes_transferred;
  std::vector<char> response_data;
  std::string file_path = "./request_handler_tests/good_request_with_response_mime_zip_test";
  std::ifstream file(file_path.c_str(), std::ios::in | std::ios::binary);
  std::string body = "";

  char c;
  while (file.get(c))
    body += c;

  file.close();

  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req{http::verb::get, "/static1/mime_zip_test.zip", 10};
  std::unique_ptr<RequestHandler> handler = nhf.createHandler(&req);
  http::response<http::string_body> res;
  handler->handle_request(req, &res);
  EXPECT_TRUE(res.body() == body);
  EXPECT_TRUE(res.result() == http::status::ok);
}

TEST_F(RequestHandlerTest, good_static1_zip_test_request)
{
  size_t bytes_transferred;
  std::vector<char> response_data;
  std::string file_path = "./request_handler_tests/good_request_with_response_static1_zip_test";
  std::ifstream file(file_path.c_str(), std::ios::in | std::ios::binary);
  std::string body = "";

  char c;
  while (file.get(c))
    body += c;

  file.close();

  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req{http::verb::get, "/static1/ziptest.pdf", 10};
  std::unique_ptr<RequestHandler> handler = nhf.createHandler(&req);
  http::response<http::string_body> res;
  handler->handle_request(req, &res);
  EXPECT_TRUE(res.body() == body);
  EXPECT_TRUE(res.result() == http::status::ok);
}

TEST_F(RequestHandlerTest, good_static2_mime_gif_request)
{
  size_t bytes_transferred;
  std::vector<char> response_data;
  std::string file_path = "./request_handler_tests/good_request_with_response_static2_mime_gif_test";
  std::ifstream file(file_path.c_str(), std::ios::in | std::ios::binary);
  std::string body = "";

  char c;
  while (file.get(c))
    body += c;

  file.close();

  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req{http::verb::get, "/static2/mime_gif_test.gif", 10};
  std::unique_ptr<RequestHandler> handler = nhf.createHandler(&req);
  http::response<http::string_body> res;
  handler->handle_request(req, &res);
  EXPECT_TRUE(res.body() == body);
  EXPECT_TRUE(res.result() == http::status::ok);
}

TEST_F(RequestHandlerTest, bad_static_request)
{
  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req{http::verb::get, "/static/does_not_exist.jpeg", 10};
  std::unique_ptr<RequestHandler> handler = nhf.createHandler(&req);
  http::response<http::string_body> res;
  handler->handle_request(req, &res);
  EXPECT_TRUE(res.result() == http::status::not_found);
}

TEST_F(RequestHandlerTest, default_400_request_empty_request)
{
  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req{http::verb::get, "/", 10};
  std::unique_ptr<RequestHandler> handler = nhf.createHandler(nullptr);
  http::response<http::string_body> res;
  handler->handle_request(req, &res);
  EXPECT_TRUE(res.body() == "");
  EXPECT_TRUE(res.result() == http::status::bad_request);
}

TEST_F(RequestHandlerTest, invalid_uri_request_404)
{
  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req{http::verb::get, "/error", 10};
  std::unique_ptr<RequestHandler> handler = nhf.createHandler(&req);
  http::response<http::string_body> res;
  handler->handle_request(req, &res);
  EXPECT_TRUE(res.body() == "");
  EXPECT_TRUE(res.result() == http::status::not_found);
}

TEST_F(RequestHandlerTest, invalid_uri_static_request_404)
{
  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req{http::verb::get, "/static/not_real_file.pdf", 10};
  std::unique_ptr<RequestHandler> handler = nhf.createHandler(&req);
  http::response<http::string_body> res;
  bool status = handler->handle_request(req, &res);
  EXPECT_TRUE(res.body() == "");
  EXPECT_TRUE(res.result() == http::status::not_found);
}

TEST_F(RequestHandlerTest, empty_path_uri_request_404)
{
  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req{http::verb::get, "", 10};
  std::unique_ptr<RequestHandler> handler = nhf.createHandler(&req);
  http::response<http::string_body> res;
  handler->handle_request(req, &res);
  EXPECT_TRUE(res.body() == "");
  EXPECT_TRUE(res.result() == http::status::not_found);
}

// These must be combined so that the data_path is not reset between tests
TEST_F(RequestHandlerTest, valid_crud_post_get)
{
  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req_1{http::verb::post, "/api/Shoes", 10};
  req_1.body() = "{\"name\": \"Shoe Name\", \"size\": 10}";

  std::unique_ptr<RequestHandler> handler_1 = nhf.createHandler(&req_1);
  http::response<http::string_body> res_1;
  handler_1->handle_request(req_1, &res_1);

  EXPECT_TRUE(res_1.body() == "{\"id\": 1}");
  EXPECT_TRUE(res_1.result() == http::status::created);

  http::request<http::string_body> req_2{http::verb::get, "/api/Shoes/1", 10};
  std::unique_ptr<RequestHandler> handler_2 = nhf.createHandler(&req_2);
  http::response<http::string_body> res_2;
  handler_2->handle_request(req_2, &res_2);

  // The returned body should be same as the one in the POST request test above
  EXPECT_TRUE(res_2.body() == "{\"name\": \"Shoe Name\", \"size\": 10}");
  EXPECT_TRUE(res_2.result() == http::status::ok);
}

TEST_F(RequestHandlerTest, bad_crud_get)
{
  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req{http::verb::get, "/api/Shoes/invalid_id", 10};

  std::unique_ptr<RequestHandler> handler = nhf.createHandler(&req);
  http::response<http::string_body> res;
  handler->handle_request(req, &res);

  EXPECT_TRUE(res.body() == "404 Not Found: The directory does not exist.");
  EXPECT_TRUE(res.result() == http::status::not_found);
}

TEST_F(RequestHandlerTest, not_found_crud_get)
{
  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req{http::verb::get, "/api/Shoes/2", 10};

  std::unique_ptr<RequestHandler> handler = nhf.createHandler(&req);
  http::response<http::string_body> res;
  handler->handle_request(req, &res);
  std::cout << res.body() << std::endl;

  EXPECT_TRUE(res.body() == "404 Not Found: The ID does not exist.");
  EXPECT_TRUE(res.result() == http::status::not_found);
}

TEST_F(RequestHandlerTest, bad_crud_delete)
{
  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req{http::verb::delete_, "/api/Shoes/invalid_id", 10};

  std::unique_ptr<RequestHandler> handler = nhf.createHandler(&req);
  http::response<http::string_body> res;
  handler->handle_request(req, &res);

  EXPECT_TRUE(res.body() == "400: Bad Request. The ID is invalid.");
  EXPECT_TRUE(res.result() == http::status::bad_request);
}

TEST_F(RequestHandlerTest, crud_no_content)
{
  RequestHandlerFactory nhf(out_config);
  // request with valid id that will never exist
  http::request<http::string_body> req{http::verb::delete_, "/api/Shoes/-1", 10};

  std::unique_ptr<RequestHandler> handler = nhf.createHandler(&req);
  http::response<http::string_body> res;
  handler->handle_request(req, &res);

  EXPECT_TRUE(res.body().empty());
  EXPECT_TRUE(res.result() == http::status::no_content);
}

TEST_F(RequestHandlerTest, crud_delete_success)
{
  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req_1{http::verb::post, "/api/Shoes", 10};
  req_1.body() = "{\"name\": \"Shoe Name\", \"size\": 10}";

  std::unique_ptr<RequestHandler> handler_1 = nhf.createHandler(&req_1);
  http::response<http::string_body> res_1;
  handler_1->handle_request(req_1, &res_1);

  EXPECT_TRUE(res_1.body() == "{\"id\": 1}");
  EXPECT_TRUE(res_1.result() == http::status::created);

  http::request<http::string_body> req_2{http::verb::delete_, "/api/Shoes/1", 10};
  std::unique_ptr<RequestHandler> handler_2 = nhf.createHandler(&req_2);
  http::response<http::string_body> res_2;
  handler_2->handle_request(req_2, &res_2);

  // The returned body should be same as the one in the POST request test above
  EXPECT_TRUE(res_2.body() == "File " + req_2.target().to_string() + " deleted successfully.");
  EXPECT_TRUE(res_2.result() == http::status::ok);
}

TEST_F(RequestHandlerTest, bad_crud_verb)
{
  RequestHandlerFactory nhf(out_config);

  // We do not support the CONNECT Verb
  http::request<http::string_body> req{http::verb::connect, "/api/Shoes/2", 10};

  std::unique_ptr<RequestHandler> handler = nhf.createHandler(&req);
  http::response<http::string_body> res;
  handler->handle_request(req, &res);
  std::cout << res.body() << std::endl;

  EXPECT_TRUE(res.result() == http::status::method_not_allowed);
}

TEST_F(RequestHandlerTest, valid_crud_put_get)
{
  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req_1{http::verb::put, "/api/Shoes/1", 10};
  req_1.body() = "{\"name\": \"Shoe Name\", \"size\": 10}";

  std::unique_ptr<RequestHandler> handler_1 = nhf.createHandler(&req_1);

  http::response<http::string_body> res_1;
  handler_1->handle_request(req_1, &res_1);
  EXPECT_TRUE(res_1.result() == http::status::created);

  http::request<http::string_body> req_2{http::verb::get, "/api/Shoes/1", 10};
  std::unique_ptr<RequestHandler> handler_2 = nhf.createHandler(&req_2);
  http::response<http::string_body> res_2;
  handler_2->handle_request(req_2, &res_2);
  EXPECT_TRUE(res_2.body() == "{\"name\": \"Shoe Name\", \"size\": 10}");
  EXPECT_TRUE(res_2.result() == http::status::ok);

  http::request<http::string_body> req_3{http::verb::put, "/api/Shoes/1", 10};
  req_3.body() = "{\"name\": \"Shoe Name\", \"size\": 10}";

  std::unique_ptr<RequestHandler> handler_3 = nhf.createHandler(&req_3);

  http::response<http::string_body> res_3;
  handler_3->handle_request(req_3, &res_3);
  EXPECT_TRUE(res_3.result() == http::status::ok);

  http::request<http::string_body> req_4{http::verb::get, "/api/Shoes/1", 10};
  std::unique_ptr<RequestHandler> handler_4 = nhf.createHandler(&req_4);
  http::response<http::string_body> res_4;
  handler_4->handle_request(req_4, &res_4);
  EXPECT_TRUE(res_4.body() == "{\"name\": \"Shoe Name\", \"size\": 10}");
  EXPECT_TRUE(res_4.result() == http::status::ok);
}

TEST_F(RequestHandlerTest, invalid_crud_put_invalid_id)
{
  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req{http::verb::put, "/api/Shoes/invalid_id", 10};

  std::unique_ptr<RequestHandler> handler = nhf.createHandler(&req);
  http::response<http::string_body> res;
  handler->handle_request(req, &res);

  EXPECT_TRUE(res.body() == "400: Bad Request. The ID is invalid.");
  EXPECT_TRUE(res.result() == http::status::bad_request);
}

TEST_F(RequestHandlerTest, valid_crud_get_single_item_from_directory)
{
  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req_1{http::verb::put, "/api/Shoes/1", 10};
  req_1.body() = "{\"name\": \"Shoe Name\", \"size\": 10}";

  std::unique_ptr<RequestHandler> handler_1 = nhf.createHandler(&req_1);

  http::response<http::string_body> res_1;
  handler_1->handle_request(req_1, &res_1);
  EXPECT_TRUE(res_1.result() == http::status::created);

  http::request<http::string_body> req_2{http::verb::get, "/api/Shoes", 10};
  std::unique_ptr<RequestHandler> handler_2 = nhf.createHandler(&req_2);
  http::response<http::string_body> res_2;
  handler_2->handle_request(req_2, &res_2);
  EXPECT_TRUE(res_2.body() == "[1]");
  EXPECT_TRUE(res_2.result() == http::status::ok);
}

TEST_F(RequestHandlerTest, valid_crud_get_multiple_items_from_directory)
{
  RequestHandlerFactory nhf(out_config);

  std::vector<std::string> filenames{"1", "4", "7", "3", "12", "16", "8", "100", "1000", "1043", "129543"};
  std::set<std::string> filenames_set(filenames.begin(), filenames.end());
  for (auto it = filenames.begin(); it != filenames.end(); it++)
  {
    http::request<http::string_body> req{http::verb::put, "/api/Shoes/" + *it, 10};
    req.body() = "{\"name\": \"Shoe Name\", \"size\": 10}";
    std::unique_ptr<RequestHandler> handler_1 = nhf.createHandler(&req);
    http::response<http::string_body> res;
    handler_1->handle_request(req, &res);
    EXPECT_TRUE(res.result() == http::status::created);
  }

  http::request<http::string_body> req{http::verb::get, "/api/Shoes", 10};
  std::unique_ptr<RequestHandler> handler = nhf.createHandler(&req);
  http::response<http::string_body> res;
  handler->handle_request(req, &res);
  EXPECT_TRUE(res.result() == http::status::ok);
  EXPECT_TRUE(res.body().front() == '[');
  EXPECT_TRUE(res.body().back() == ']');
  res.body().pop_back();
  res.body().erase(0);

  std::vector<std::string> result;
  std::stringstream ss(res.body());
  std::string item;

  while (getline(ss, item, ','))
  {
    EXPECT_TRUE(filenames_set.find(item) != filenames_set.end());
  }
}

TEST_F(RequestHandlerTest, good_health_request)
{
  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req{http::verb::get, "/health", 10};
  std::unique_ptr<RequestHandler> handler = nhf.createHandler(&req);
  http::response<http::string_body> res;
  handler->handle_request(req, &res);
  std::ostringstream oss;
  oss << "OK";
  EXPECT_TRUE(res.body() == oss.str());
  EXPECT_TRUE(res.result() == http::status::ok);
}

TEST_F(RequestHandlerTest, sleep_request)
{
  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req{http::verb::get, "/sleep", 10};
  std::unique_ptr<RequestHandler> handler = nhf.createHandler(&req);
  http::response<http::string_body> res;
  handler->handle_request(req, &res);
  sleep(2);
  EXPECT_TRUE(res.body() == "");
  EXPECT_TRUE(res.result() == http::status::ok);
}

TEST_F(RequestHandlerTest, multiple_thread_request_echo)
{
  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req{http::verb::get, "/sleep", 10};
  std::unique_ptr<RequestHandler> handler = nhf.createHandler(&req);
  http::response<http::string_body> res;
  handler->handle_request(req, &res);

  http::request<http::string_body> req2{http::verb::get, "/echo", 10};
  std::unique_ptr<RequestHandler> handler2 = nhf.createHandler(&req2);
  http::response<http::string_body> res2;
  handler2->handle_request(req2, &res2);

  std::ostringstream oss;
  oss << req2;
  EXPECT_TRUE(res2.body() == oss.str());
  EXPECT_TRUE(res2.result() == http::status::ok);

  sleep(2);

  EXPECT_TRUE(res.body() == "");
  EXPECT_TRUE(res.result() == http::status::ok);
}

TEST_F(RequestHandlerTest, multiple_thread_request_static)
{
  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req{http::verb::get, "/sleep", 10};
  std::unique_ptr<RequestHandler> handler = nhf.createHandler(&req);
  http::response<http::string_body> res;
  handler->handle_request(req, &res);

  size_t bytes_transferred;
  std::vector<char> response_data;
  std::string file_path = "./request_handler_tests/good_request_with_response_mime_jpeg_test";
  std::ifstream file(file_path.c_str(), std::ios::in | std::ios::binary);
  std::string body = "";

  char c;
  while (file.get(c))
    body += c;

  file.close();

  http::request<http::string_body> req2{http::verb::get, "/static/mime_jpeg_test.jpeg", 10};
  std::unique_ptr<RequestHandler> handler2 = nhf.createHandler(&req2);
  http::response<http::string_body> res2;
  handler2->handle_request(req2, &res2);
  EXPECT_TRUE(res2.body() == body);
  EXPECT_TRUE(res2.result() == http::status::ok);

  sleep(2);

  EXPECT_TRUE(res.body() == "");
  EXPECT_TRUE(res.result() == http::status::ok);
}

TEST_F(RequestHandlerTest, multiple_thread_request_crud)
{
  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req{http::verb::get, "/sleep", 10};
  std::unique_ptr<RequestHandler> handler = nhf.createHandler(&req);
  http::response<http::string_body> res;
  handler->handle_request(req, &res);

  http::request<http::string_body> req2{http::verb::get, "/api/Shoes/2", 10};

  std::unique_ptr<RequestHandler> handler2 = nhf.createHandler(&req2);
  http::response<http::string_body> res2;
  handler2->handle_request(req2, &res2);

  EXPECT_TRUE(res2.body() == "404 Not Found: The ID does not exist.");
  EXPECT_TRUE(res2.result() == http::status::not_found);

  sleep(2);

  EXPECT_TRUE(res.body() == "");
  EXPECT_TRUE(res.result() == http::status::ok);
}

TEST_F(RequestHandlerTest, bad_crud_incorrect_length_uri_too_long)
{
  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req{http::verb::post, "/api/Shoes/big/sock/2", 10};
  std::unique_ptr<RequestHandler> handler = nhf.createHandler(&req);
  http::response<http::string_body> res;
  handler->handle_request(req, &res);

  EXPECT_TRUE(res.body() == "400: Bad Request. The ID is invalid.");
  EXPECT_TRUE(res.result() == http::status::bad_request);
}

TEST_F(RequestHandlerTest, bad_crud_incorrect_length_uri_short)
{
  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req{http::verb::post, "/api", 10};
  std::unique_ptr<RequestHandler> handler = nhf.createHandler(&req);
  http::response<http::string_body> res;
  handler->handle_request(req, &res);

  EXPECT_TRUE(res.body() == "400: Bad Request. The ID is invalid.");
  EXPECT_TRUE(res.result() == http::status::bad_request);
}

TEST_F(RequestHandlerTest, bad_crud_incorrect_uri_get)
{
  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req{http::verb::get, "/api/shoes/", 10};
  std::unique_ptr<RequestHandler> handler = nhf.createHandler(&req);
  http::response<http::string_body> res;
  handler->handle_request(req, &res);

  EXPECT_TRUE(res.body() == "400: Bad Request. The ID is invalid.");
  EXPECT_TRUE(res.result() == http::status::bad_request);
}

TEST_F(RequestHandlerTest, bad_crud_incorrect_uri_put)
{
  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req{http::verb::put, "/api/shoes/", 10};
  std::unique_ptr<RequestHandler> handler = nhf.createHandler(&req);
  http::response<http::string_body> res;
  handler->handle_request(req, &res);

  EXPECT_TRUE(res.body() == "400: Bad Request. The ID is invalid.");
  EXPECT_TRUE(res.result() == http::status::bad_request);
}

TEST_F(RequestHandlerTest, bad_crud_incorrect_uri_delete)
{
  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req{http::verb::delete_, "/api/shoes/", 10};
  std::unique_ptr<RequestHandler> handler = nhf.createHandler(&req);
  http::response<http::string_body> res;
  handler->handle_request(req, &res);

  EXPECT_TRUE(res.body() == "400: Bad Request. The ID is invalid.");
  EXPECT_TRUE(res.result() == http::status::bad_request);
}

TEST_F(RequestHandlerTest, get_name_of_echo_request_handler)
{
  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req{http::verb::get, "/echo/", 10};
  std::unique_ptr<RequestHandler> handler = nhf.createHandler(&req);
  std::string name = handler->get_name();
  std::string expected_name = "EchoRequestHandler";

  EXPECT_TRUE(name == expected_name);
}

TEST_F(RequestHandlerTest, get_name_of_static_request_handler)
{
  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req{http::verb::get, "/static/index.html", 10};
  std::unique_ptr<RequestHandler> handler = nhf.createHandler(&req);
  std::string name = handler->get_name();
  std::string expected_name = "StaticRequestHandler";

  EXPECT_TRUE(name == expected_name);
}

TEST_F(RequestHandlerTest, get_name_of_bad_request_handler)
{
  RequestHandlerFactory nhf(out_config);
  std::unique_ptr<RequestHandler> handler = nhf.createHandler(nullptr);
  std::string name = handler->get_name();
  std::string expected_name = "BadRequestHandler";

  EXPECT_TRUE(name == expected_name);
}

TEST_F(RequestHandlerTest, get_name_of_not_found_request_handler)
{
  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req{http::verb::get, "/connect", 10};
  std::unique_ptr<RequestHandler> handler = nhf.createHandler(&req);
  std::string name = handler->get_name();
  std::string expected_name = "NotFoundRequestHandler";

  EXPECT_TRUE(name == expected_name);
}

TEST_F(RequestHandlerTest, get_name_of_crud_request_handler)
{
  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req{http::verb::get, "/api/Shoes/2", 10};
  std::unique_ptr<RequestHandler> handler = nhf.createHandler(&req);
  std::string name = handler->get_name();
  std::string expected_name = "CrudRequestHandler";

  EXPECT_TRUE(name == expected_name);
}

TEST_F(RequestHandlerTest, get_name_of_health_request_handler)
{
  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req{http::verb::get, "/health", 10};
  std::unique_ptr<RequestHandler> handler = nhf.createHandler(&req);
  std::string name = handler->get_name();
  std::string expected_name = "HealthRequestHandler";

  EXPECT_TRUE(name == expected_name);
}

TEST_F(RequestHandlerTest, get_name_of_sleep_request_handler)
{
  RequestHandlerFactory nhf(out_config);
  http::request<http::string_body> req{http::verb::get, "/sleep", 10};
  std::unique_ptr<RequestHandler> handler = nhf.createHandler(&req);
  std::string name = handler->get_name();
  std::string expected_name = "SleepRequestHandler";

  EXPECT_TRUE(name == expected_name);
}