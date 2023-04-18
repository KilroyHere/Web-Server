#include "gtest/gtest.h"
#include "config_parser.h"

const int port_num = 8080;

class ConfigParserTest : public ::testing::Test
{
protected:
  NginxConfigParser parser;
  NginxConfig out_config;
  std::vector<std::string> query{"server", "listen"};
};

// Config File Tests
TEST_F(ConfigParserTest, no_semi_colon_config)
{
  bool failure = parser.Parse("config_parser_tests/no_semi_colon_config", &out_config);
  EXPECT_FALSE(failure);
}

TEST_F(ConfigParserTest, extra_right_brace_config)
{
  bool failure = parser.Parse("config_parser_tests/extra_right_brace_config", &out_config);
  EXPECT_FALSE(failure);
}

TEST_F(ConfigParserTest, extra_left_brace_config)
{
  bool failure = parser.Parse("config_parser_tests/extra_left_brace_config", &out_config);
  EXPECT_FALSE(failure);
}

TEST_F(ConfigParserTest, empty_config)
{
  bool success = parser.Parse("config_parser_tests/empty_config", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(ConfigParserTest, empty_server_config)
{
  bool success = parser.Parse("config_parser_tests/empty_server_config", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(ConfigParserTest, invalid_quotation_config)
{
  bool failure = parser.Parse("config_parser_tests/invalid_quotation_config", &out_config);
  EXPECT_FALSE(failure);
}

TEST_F(ConfigParserTest, invalid_location_config)
{
  bool failure = parser.Parse("config_parser_tests/invalid_location_config", &out_config);
  EXPECT_FALSE(failure);
}

TEST_F(ConfigParserTest, comment_config)
{
  bool success = parser.Parse("config_parser_tests/comment_config", &out_config);
  EXPECT_TRUE(success);
}

// Config Port Tests
TEST_F(ConfigParserTest, correct_port_config)
{
  bool success = parser.Parse("config_parser_tests/correct_port_config", &out_config);
  std::string port;
  bool found_port = out_config.config_port_num(query, port);
  bool result = success && atoi(port.c_str()) == port_num && found_port;
  EXPECT_TRUE(result);
}

TEST_F(ConfigParserTest, incorrect_port_config)
{
  bool success = parser.Parse("config_parser_tests/incorrect_port_config", &out_config);
  std::string port;
  bool no_port = out_config.config_port_num(query, port);
  EXPECT_FALSE(no_port);
}

TEST_F(ConfigParserTest, invalid_port_config)
{
  bool success = parser.Parse("config_parser_tests/invalid_port_config", &out_config);
  std::string port;
  bool no_port = out_config.config_port_num(query, port);
  EXPECT_FALSE(no_port);
}

TEST_F(ConfigParserTest, nonnumber_port_config)
{
  bool success = parser.Parse("config_parser_tests/nonnumber_port_config", &out_config);
  std::string port;
  bool no_port = out_config.config_port_num(query, port);
  EXPECT_FALSE(no_port);
}

TEST_F(ConfigParserTest, empty_port_config)
{
  bool success = parser.Parse("config_parser_tests/empty_port_config", &out_config);
  std::string port;
  bool no_port = out_config.config_port_num(query, port);
  EXPECT_FALSE(no_port);
}

TEST_F(ConfigParserTest, several_server_config)
{
  bool success = parser.Parse("config_parser_tests/several_server_config", &out_config);
  std::string port;
  bool found_port = out_config.config_port_num(query, port);
  bool result = success && atoi(port.c_str()) == port_num && found_port;
  EXPECT_TRUE(result);
}

// query_config Tests
class QueryConfigTest : public ::testing::Test
{
protected:
  NginxConfigParser parser;
  NginxConfig out_config;
};

TEST_F(QueryConfigTest, empty_config)
{
  std::vector<std::string> query{"server", "listen"};
  bool success = parser.Parse("config_parser_tests/empty_config", &out_config);
  std::string port;
  bool no_port = out_config.query_config(query, port);
  EXPECT_FALSE(no_port);
}

TEST_F(QueryConfigTest, missing_discreptive_config)
{
  std::vector<std::string> query{"http", "server"};
  bool success = parser.Parse("config_parser_tests/missing_discreptive_config", &out_config);
  std::string port;
  bool no_port = out_config.query_config(query, port);
  EXPECT_FALSE(no_port);
}

TEST_F(QueryConfigTest, example_config)
{
  std::vector<std::string> query{"http", "server", "listen"};
  bool success = parser.Parse("config_parser_tests/example_config", &out_config);
  std::string port;
  bool found_port = out_config.query_config(query, port);
  bool result = success && atoi(port.c_str()) == port_num && found_port;
  EXPECT_TRUE(result);
}

// relative_path_query Tests

TEST_F(ConfigParserTest, nested_config)
{
  std::vector<std::string> query{"server", "listen"};
  bool success = parser.Parse("config_parser_tests/nested_config", &out_config);
  std::string port;
  bool found_port = out_config.relative_path_query(query, port, 0);
  bool result = success && atoi(port.c_str()) == port_num && found_port;
  EXPECT_TRUE(result);
}

TEST_F(ConfigParserTest, no_path_config)
{
  std::vector<std::string> query{"http", "server", "listen"};
  bool success = parser.Parse("config_parser_tests/no_path_config", &out_config);
  std::string port;
  bool no_path = out_config.relative_path_query(query, port, 0);
  EXPECT_FALSE(no_path);
}
