#include <boost/asio/ip/tcp.hpp>
#include <gtest/gtest.h>
#include "session.h"
#include "config_parser.h"
#include <vector>

class SessionTest : public ::testing::Test
{
    protected:
        boost::asio::io_service io_service;
        NginxConfigParser parser;
        NginxConfig config;
        
};

// TODO:
TEST_F(SessionTest, async_read_test) 
{
    bool success = parser.Parse("config_parser_tests/deploy_config", &config);
    session session_(io_service, config);
    session_.async_read();
    EXPECT_TRUE(true);
}

TEST_F(SessionTest, async_write_test)
{
    bool success = parser.Parse("config_parser_tests/deploy_config", &config);
    session session_(io_service, config);
    std::vector<char> v;
    session_.async_write(v);
    EXPECT_TRUE(true);
}

TEST_F(SessionTest, get_socket_test)
{
    bool success = parser.Parse("config_parser_tests/deploy_config", &config);
    session session_(io_service, config);
    session_.get_socket();
    EXPECT_TRUE(true);
}



