#include <boost/asio/ip/tcp.hpp>
#include <gtest/gtest.h>
#include "session.h"
#include <vector>

class SessionTest : public ::testing::Test
{
    protected:
        boost::asio::io_service io_service;
        
};

// TODO:
TEST_F(SessionTest, async_read_test) 
{
    session session_(io_service);
    session_.async_read();
    EXPECT_TRUE(true);
}

TEST_F(SessionTest, async_write_test)
{
    session session_(io_service);
    std::vector<char> v;
    session_.async_write(v);
    EXPECT_TRUE(true);
}

TEST_F(SessionTest, get_socket_test)
{
    session session_(io_service);
    session_.get_socket();
    EXPECT_TRUE(true);
}



