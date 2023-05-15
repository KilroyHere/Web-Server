#ifndef SESSION_H
#define SESSION_H

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <boost/log/trivial.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/http/status.hpp>
#include <boost/beast/core/buffers_to_string.hpp>
// #include "request_handler.h"
#include "request_parser.h"
#include "config_parser.h"
#include "new_request_handler.h"
#include "request_handler_factory.h"

using boost::asio::ip::tcp;

class session
{
public:
    // Tests
    friend class SessionTest;

    ~session();
    session(boost::asio::io_service &io_service, NginxConfig config);
    // TODO: Old Constructor. No Action required. Need to remove eventually.
    // session(boost::asio::io_service &io_service);
    // Gets the socket associated with this session
    tcp::socket &get_socket();
    // Read from the socket
    void async_read();
    // Write to the socket
    void async_write(const std::vector<char> &response);

private:
    // Callback for async_read, handles the read data
    int handle_read(const boost::system::error_code &error, size_t bytes_transferred);
    // Callback for async_write
    int handle_write(const boost::system::error_code &error);
    // Set the content of boost::beast::http:request
    void set_http_request();
    // Read the body of a request
    bool session::read_body(std::vector<char> data, size_t bytes_transferred);
    // Parse data into request
    RequestParser::result_type session::request_parse(size_t bytes_transferred);
    // Reset all request and response
    void reset();

    tcp::socket socket_;
    size_t max_buffer_size = 1024;
    std::vector<char> data_;
    int body_read_;
    Request request_;
    RequestParser parser_;
    NginxConfig config_;
    RequestHandlerFactory request_handler_factory_;
    beast::http::request<http::string_body> http_request_;
    beast::http::response<http::string_body> http_response_;

    enum parser_state
    {
        READING_HEADER,
        READING_BODY
    } parsing_state_;
};

#endif // SESSION_H