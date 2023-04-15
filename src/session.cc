#include "session.h"

session::session(boost::asio::io_service &io_service)
    : socket_(io_service)
{
  data_.resize(max_buffer_size);
}

tcp::socket &session::get_socket()
{
  return socket_;
}

void session::start()
{
  socket_.async_read_some(boost::asio::buffer(data_),
                          boost::bind(&session::handle_read, this,
                                      boost::asio::placeholders::error,
                                      boost::asio::placeholders::bytes_transferred));
}

void session::handle_read(const boost::system::error_code &error, size_t bytes_transferred)
{
  if (!error)
  {

    for (int i = 0; i < bytes_transferred; i++)
    {
      std::cerr << data_[i];
    }
    request_parser::result_type result = parser_.parse(request_, data_, bytes_transferred);

    // If read the header successfully
    if (result == request_parser::good)
    {
      // Header Read
      std::cerr << "\n==================\nGOOD REQUEST!!\n==================\n";
      int read_from = parser_.read_from_;
      parser_.reset();
      request_.set_headers_map();

      //  Check for content-length and parse body
      if (request_.headers_map.find("content-length") != request_.headers_map.end())
      {
        std::string content_length = request_.headers_map["content-length"];

        read_body(read_from, bytes_transferred, stoi(content_length));
      }

      // Echo response
      std::string response_body = request_.request_headers + request_.request_body;
      response_.set_echo_response(200, response_body);
      boost::asio::async_write(socket_, boost::asio::buffer(response_.to_buffer()),
                               boost::bind(&session::handle_write, this, boost::asio::placeholders::error));

      // If connection:close, end connection
      if (request_.headers_map.find("connection") != request_.headers_map.end())
      {
        if (request_.headers_map["connection"] == "close")
        {
          socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
          delete this;
          return;
        }
      }

      // Ready for a new request in the same session
      request_.purge_request();
      response_.purge_response();
      start();
    }
    // If the HTTP request is malformed
    else if (result == request_parser::bad)
    {
      std::cerr << "\n==================\nBAD_REQUEST!!\n==================\n";
      parser_.reset();
      // TODO: Respond (400 Bad Request)
      // Close connection
      socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
      delete this;
      return;
    }
    // If the parsing of the header is not yet complete
    else
    {
      start();
    }
  }
  else
  {
    // Get an EOF error when connection set to be alive and no further requests sent
    if ((boost::asio::error::eof != error) &&
        (boost::asio::error::connection_reset != error))
    {
      std::cerr << error << ": Error reading from TCP socket";
    }
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
    delete this;
    return;
  }
}

void session::read_body(int read_from, int bytes_transferred, int content_length)
{
  for (int i = read_from; i < bytes_transferred; i++)
  {
    request_.request_body.push_back(data_[i]);
  }
  // TODO: if the body does not fit into this buffer, keep reading.
}

void session::handle_write(const boost::system::error_code &error)
{
  if (!error)
  {
  }
  else
  {
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
    delete this;
  }
}