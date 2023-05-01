// Adapted from:
// http_response.h
// ~~~~~~~~~
// Adapted from:
// Copyright (c) 2003-2020 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <boost/asio.hpp>


struct response_header
{
    std::string name;
    std::string value;
};

/// A response to be sent to a client.
class Response
{
public:
    // Sets the echo response
    void set_echo_response(int status, const std::string response_body);
    // Converts the response to vector<char> 
    std::vector<char> to_buffer();
    // Clear all response fields
    void purge_response();

    //handle static file inputs
    void set_file_response(int status, const std::string file, std::string file_type);

private:
    // The headers to be included in the response.
    std::vector<response_header> headers_;
    // The content to be sent in the response.
    std::string content_;
    // The status code of the response.
    int status_code_;

    // Collection of all possible status strings
    struct status
    {
        const std::string ok =
            "HTTP/1.1 200 OK\r\n";
        const std::string created =
            "HTTP/1.1 201 Created\r\n";
        const std::string accepted =
            "HTTP/1.1 202 Accepted\r\n";
        const std::string no_content =
            "HTTP/1.1 204 No Content\r\n";
        const std::string multiple_choices =
            "HTTP/1.1 300 Multiple Choices\r\n";
        const std::string moved_permanently =
            "HTTP/1.1 301 Moved Permanently\r\n";
        const std::string moved_temporarily =
            "HTTP/1.1 302 Moved Temporarily\r\n";
        const std::string not_modified =
            "HTTP/1.1 304 Not Modified\r\n";
        const std::string bad_request =
            "HTTP/1.1 400 Bad Request\r\n";
        const std::string unauthorized =
            "HTTP/1.1 401 Unauthorized\r\n";
        const std::string forbidden =
            "HTTP/1.1 403 Forbidden\r\n";
        const std::string not_found =
            "HTTP/1.1 404 Not Found\r\n";
        const std::string internal_server_error =
            "HTTP/1.1 500 Internal Server Error\r\n";
        const std::string not_implemented =
            "HTTP/1.1 501 Not Implemented\r\n";
        const std::string bad_gateway =
            "HTTP/1.1 502 Bad Gateway\r\n";
        const std::string service_unavailable =
            "HTTP/1.1 503 Service Unavailable\r\n";
    } status_strings;

    // Mapping of status code to status strings
    std::map<int, std::string> status_map_ =
        {
            {200, status_strings.ok},
            {201, status_strings.created},
            {202, status_strings.accepted},
            {204, status_strings.no_content},
            {300, status_strings.multiple_choices},
            {301, status_strings.moved_permanently},
            {302, status_strings.moved_temporarily},
            {304, status_strings.not_modified},
            {400, status_strings.bad_request},
            {401, status_strings.unauthorized},
            {403, status_strings.forbidden},
            {404, status_strings.not_found},
            {500, status_strings.internal_server_error},
            {501, status_strings.not_implemented},
            {502, status_strings.bad_gateway},
            {503, status_strings.service_unavailable}};

    // Converts the status code to a HTTP response header
    std::string status_to_string(int status);
};

#endif // HTTP_RESPONSE_H