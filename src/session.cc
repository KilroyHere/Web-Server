#include "session.h"

session::session(boost::asio::io_service &io_service, NginxConfig config)
    : socket_(io_service), config_(config), request_handler_factory_(config)
{
    data_.resize(max_buffer_size);
    parsing_state_ = READING_HEADER;
}

session::~session()
{
    socket_.close();
    BOOST_LOG_TRIVIAL(info) << "Run session destructor";
}

tcp::socket &session::get_socket()
{
    return socket_;
}

void session::async_read()
{
    socket_.async_read_some(boost::asio::buffer(data_),
                            boost::bind(&session::handle_read, this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
}

int session::handle_read(const boost::system::error_code &error, size_t bytes_transferred)
{
    if (!error)
    {
        RequestParser::result_type parse_result = request_parse(bytes_transferred);

        if (parse_result != RequestParser::indeterminate)
        {
            // Create a Request Handler
            std::unique_ptr<RequestHandler> handler = nullptr;

            if (parse_result == RequestParser::good)
            {
                set_http_request();
                // LOGGING:
                std::string log_message = "==================REQUEST===================\n";
                log_message += "Client: " + boost::lexical_cast<std::string>(socket_.remote_endpoint());
                log_message += "\n--------------------------------------------\n";
                std::ostringstream request_oss;
                request_oss << http_request_.base();
                log_message += request_oss.str();
                log_message += "\n===============END-OF-REQUEST===============";
                BOOST_LOG_TRIVIAL(info) << log_message;
                BOOST_LOG_TRIVIAL(info) << "===============GOOD REQUEST!!===============";
                handler = request_handler_factory_.createHandler(&http_request_);
            }
            else
            {
                // LOGGING:
                std::string log_message = "==================REQUEST===================\n";
                log_message += "Client: " + boost::lexical_cast<std::string>(socket_.remote_endpoint());
                BOOST_LOG_TRIVIAL(info) << log_message;
                BOOST_LOG_TRIVIAL(severity_level::info) << "===============BAD REQUEST!!===============";
                handler = std::make_unique<BadRequestHandler>("", config_);
            }
            // Handle the request
            bool request_handled = handler->handle_request(http_request_, &http_response_);

            // If request is handled
            if (request_handled)
            {
                // Convert the response into a buffer
                std::ostringstream response_oss;
                response_oss << http_response_;
                std::string response_str = response_oss.str();

                // LOGGING:
                std::ostringstream response_header_oss;
                response_header_oss << http_response_.base();
                std::string log_message = "==================RESPONSE==================\n";
                log_message += "Client: " + boost::lexical_cast<std::string>(socket_.remote_endpoint());
                log_message += "\n--------------------------------------------\n";
                log_message += response_header_oss.str();
                log_message += "\n==============END-OF-RESPONSE===============";
                BOOST_LOG_TRIVIAL(info) << log_message;
                log_message = "[ResponseMetrics] ResponseCode: " + std::to_string(http_response_.result_int()) +
                              " | RequestPath: " + http_request_.target().to_string() +
                              " | RequestIP: " + boost::lexical_cast<std::string>(socket_.remote_endpoint()) +
                              " | RequestHandler: " + handler->get_name();
                BOOST_LOG_TRIVIAL(info) << log_message;

                std::vector<char> response_buffer(response_str.begin(), response_str.end());

                // Send the response
                async_write(response_buffer);

                // Close the connection if needed
                if (http_response_.result() != http::status::ok || !http_request_.keep_alive())
                {
                    BOOST_LOG_TRIVIAL(severity_level::info) << "Closing connection, ending session.";
                    socket_.close();
                    return 0;
                }

                // Reset all requests and response to be ready for next request
                reset();
            }
        }
        // Keep reading if request not fully parsed or request not handled or if request handled and connection not closed
        async_read();
    }
    else
    {
        // Get an EOF error when connection set to be alive and no further requests sent
        if ((boost::asio::error::eof != error) &&
            (boost::asio::error::connection_reset != error))
        {
            BOOST_LOG_TRIVIAL(severity_level::error) << error << ": Error reading from TCP socket";
        }
        socket_.close();
        return 1;
    }
    // Shouldn't reach here
    return 1;
}

void session::async_write(const std::vector<char> &response)
{
    boost::asio::async_write(socket_, boost::asio::buffer(response),
                             boost::bind(&session::handle_write, this, boost::asio::placeholders::error));
}

int session::handle_write(const boost::system::error_code &error)
{
    if (error)
    {
        socket_.close();
        BOOST_LOG_TRIVIAL(severity_level::error) << "Closing connection, ending session.";
        return 1;
    }
    return 0; // No return statement?
}

void session::set_http_request()
{
    http_request_.method_string(request_.method);
    http_request_.target(request_.uri);
    http_request_.version(request_.http_version_major * 10 + request_.http_version_minor);

    for (auto x : request_.headers_map)
    {
        http_request_.insert(x.first, x.second);
    }

    http_request_.body() = request_.request_body;
    http_request_.prepare_payload();
}

bool session::read_body(std::vector<char> data, size_t bytes_transferred)
{
    int read_from = parser_.read_from_;
    parser_.reset();
    int content_length = stoi(request_.headers_map["content-length"]);

    for (int i = read_from; i < bytes_transferred && body_read_ < content_length; i++)
    {
        request_.request_body.push_back(data.at(i));
        body_read_ += 1;
    }
    if (body_read_ == content_length)
    {
        return true;
    }
    else
    {
        return false;
    }
}

RequestParser::result_type session::request_parse(size_t bytes_transferred)
{
    if (parsing_state_ == READING_HEADER)
    {
        // Read header
        RequestParser::result_type result = parser_.parse(request_, data_, bytes_transferred);
        // Read request body if needed
        if (result == RequestParser::good)
        {
            request_.set_headers_map();
            if (request_.headers_map.find("content-length") != request_.headers_map.end())
            {
                parsing_state_ = READING_BODY;
                body_read_ = 0;
                int read_result = read_body(data_, bytes_transferred);
                if (read_result)
                {
                    return result;
                }
                else
                {
                    return RequestParser::indeterminate;
                }
            }
        }
        return result;
    }
    else
    {
        // Continue reading body
        int read_result = read_body(data_, bytes_transferred);
        if (read_result)
        {
            return RequestParser::good;
        }
        else
        {
            return RequestParser::indeterminate;
        }
    }
}

void session::reset()
{
    parsing_state_ = READING_HEADER;
    body_read_ = 0;
    request_.purge_request();
    http_request_.clear();
    http_response_.clear();
    parser_.reset();
}
