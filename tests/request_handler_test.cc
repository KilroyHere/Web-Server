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
  extract_from_file("request_handler_tests/normal_request", data, bytes_transferred);
  bool failure = handler.handle_request(data, bytes_transferred);
  EXPECT_TRUE(failure);
}
