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
            bool success = parser.Parse("config_parser_tests/deployconfig", &config);
            session_  = new session(io_service, config);
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
        NginxConfigParser parser;
        NginxConfig config;
        session *session_;

};

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