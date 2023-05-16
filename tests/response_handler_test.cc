#include "gtest/gtest.h"
#include <fstream>
#include <iostream>
#include <vector>

class ResponseHandlerTest : public ::testing::Test
{
protected:
  std::unique_ptr<ResponseHandler> handler_ = nullptr;
  Request request_;
  Response response_;

  ResponseHandlerTest()
  {
  }

  void setUpTest(std::string type)
  {
    if (type == "BadRequestResponseHandler")
      handler_ = std::make_unique<BadRequestResponseHandler>(&request_, &response_);
    else if (type == "NotFoundResponseHandler")
      handler_ = std::make_unique<NotFoundResponseHandler>(&request_, &response_);
    else if (type == "EchoResponseHandler")
    {
      request_.request_headers = "GET /echo HTTP/1.1\r\nHost: 34.83.71.241\r\nConnection: keep-alive\r\n\r\n";
      request_.request_body = "";
      handler_ = std::make_unique<EchoResponseHandler>(&request_, &response_);
    }
    else
      handler_ = std::make_unique<NotFoundResponseHandler>(&request_, &response_);

    handler_->set_response_fields();
  }

  virtual void TearDown()
  {
    std::cerr << "Tearing Down";
    response_.purge_response();
    request_.purge_request();
  }

  std::string getResponse()
  {
    std::vector<char> buffer = response_.to_buffer();
    std::string response = std::string(buffer.begin(), buffer.end());
    return response;
  }
};

TEST_F(ResponseHandlerTest, bad_request_response_test)
{
  setUpTest("BadRequestResponseHandler");
  std::string got = getResponse();
  std::string expected = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\nContent-Type: text/plain\r\n\r\n";
  EXPECT_EQ(expected, got);
}

TEST_F(ResponseHandlerTest, not_found_response_test)
{
  setUpTest("NotFoundResponseHandler");
  std::string got = getResponse();
  std::string expected = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nContent-Type: text/plain\r\n\r\n";
  EXPECT_EQ(expected, got);
}

TEST_F(ResponseHandlerTest, echo_response_test)
{
  setUpTest("EchoResponseHandler");
  std::string got = getResponse();
  std::string expected = "HTTP/1.1 200 OK\r\nContent-Length: 66\r\nContent-Type: text/plain\r\n\r\nGET /echo HTTP/1.1\r\nHost: 34.83.71.241\r\nConnection: keep-alive\r\n\r\n";
  EXPECT_EQ(expected, got);
}
