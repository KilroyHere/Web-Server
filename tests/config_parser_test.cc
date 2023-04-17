#include "gtest/gtest.h"
#include "config_parser.h"

const int port_num = 8080;

class configParserTest : public ::testing::Test {
  protected:
    NginxConfigParser parser;
    NginxConfig out_config;
    std::vector<std::string> query{"server", "listen"};
};

// Config File Tests
TEST_F(configParserTest, no_semi_colon_config) {
  bool failure = parser.Parse("no_semi_colon_config", &out_config);
  EXPECT_FALSE(failure);
}

TEST_F(configParserTest, extra_right_brace_config) {
  bool failure = parser.Parse("extra_right_brace_config", &out_config);
  EXPECT_FALSE(failure);
}

TEST_F(configParserTest, extra_left_brace_config) {
  bool failure = parser.Parse("extra_left_brace_config", &out_config);
  EXPECT_FALSE(failure);
}

TEST_F(configParserTest, empty_config) {
  bool success = parser.Parse("empty_config", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(configParserTest, empty_server_config) {
  bool success = parser.Parse("empty_server_config", &out_config);
  EXPECT_TRUE(success);
}

TEST_F (configParserTest, invalid_quotation_config) {
  bool failure = parser.Parse("invalid_quotation_config", &out_config);
  EXPECT_FALSE(failure);
}

TEST_F (configParserTest, invalid_location_config) {
  bool failure = parser.Parse("invalid_location_config", &out_config);
  EXPECT_FALSE(failure);
}

TEST_F (configParserTest, comment_config) {
  bool success = parser.Parse("comment_config", &out_config);
  EXPECT_TRUE(success);
}

//Config Port Tests
TEST_F(configParserTest, correct_port_config) {
  bool success = parser.Parse("correct_port_config", &out_config);
  std::string port;
  bool found_port = out_config.config_port_num(query, port);
  bool result = success && atoi(port.c_str()) == port_num && found_port;
  EXPECT_TRUE(result);
}

TEST_F(configParserTest, incorrect_port_config) {
  bool success = parser.Parse("incorrect_port_config", &out_config);
  std::string port;
  bool no_port = out_config.config_port_num(query, port);
  EXPECT_FALSE(no_port);
}

TEST_F(configParserTest, nonnumber_port_config) {
  bool success = parser.Parse("nonnumber_port_config", &out_config);
  std::string port;
  bool no_port = out_config.config_port_num(query, port);
  EXPECT_FALSE(no_port);
}

TEST_F(configParserTest, empty_port_config) {
  bool success = parser.Parse("empty_port_config", &out_config);
  std::string port;
  bool no_port = out_config.config_port_num(query, port);
  EXPECT_FALSE(no_port);
}

TEST_F(configParserTest, several_server_config) {
  bool success = parser.Parse("several_server_config", &out_config);
  std::string port;
  bool found_port = out_config.config_port_num(query, port);
  bool result = success && atoi(port.c_str()) == port_num && found_port;
  EXPECT_TRUE(result);
}

//query_config Tests
class queryConfigTest : public ::testing::Test {
  protected:
    NginxConfigParser parser;
    NginxConfig out_config;
};

TEST_F(queryConfigTest, empty_config) {
  std::vector<std::string> query{"server", "listen"};
  bool success = parser.Parse("empty_config", &out_config);
  std::string port;
  bool no_port = out_config.query_config(query, port);
  EXPECT_FALSE(no_port);
}

TEST_F(queryConfigTest, missing_discreptive_config) {
  std::vector<std::string> query{"http", "server"};
  bool success = parser.Parse("missing_discreptive_config", &out_config);
  std::string port;
  bool no_port = out_config.query_config(query, port);
  EXPECT_FALSE(no_port);
}

TEST_F(queryConfigTest, example_config) {
  std::vector<std::string> query{"http", "server", "listen"};
  bool success = parser.Parse("example_config", &out_config);
  std::string port;
  bool found_port = out_config.query_config(query, port);
  bool result = success && atoi(port.c_str()) == port_num && found_port;
  EXPECT_TRUE(result);
}

// relative_path_query Tests

TEST_F(configParserTest, nested_config) {
  std::vector<std::string> query{"server", "listen"};
  bool success = parser.Parse("nested_config", &out_config);
  std::string port;
  bool found_port = out_config.relative_path_query(query, port, 0);
  bool result = success && atoi(port.c_str()) == port_num && found_port;
  EXPECT_TRUE(result);
}

TEST_F(configParserTest, no_path_config) {
  std::vector<std::string> query{"http", "server", "listen"};
  bool success = parser.Parse("no_path_config", &out_config);
  std::string port;
  bool no_path = out_config.relative_path_query(query, port, 0);
  EXPECT_FALSE(no_path);
}



