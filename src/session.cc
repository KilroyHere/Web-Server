#include "session.h"

session::session(boost::asio::io_service &io_service)
    : socket_(io_service)
{
  data_.resize(max_buffer_size);
}

session::~session()
{
  socket_.close();
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

void session::handle_read(const boost::system::error_code &error, size_t bytes_transferred)
{
  if (!error)
  {
    // LOGGING:
    for (int i = 0; i < bytes_transferred; i++)
    {
      std::cerr << data_[i];
    }
    // Handle the request
    int request_handled = request_handler_.handle_request(data_, bytes_transferred);
    // If request is handled
    if (request_handled != 0)
    {
      // Set a response
      request_handler_.set_response();
      // Send the response
      async_write(request_handler_.get_response());
      // Reset the handler
      request_handler_.reset();
      // Close the connection if needed
      if (request_handler_.connection_close())
      {
        socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        delete this;
        return;
      }
    }
    // Keep reading if request not handled or if request handled and connection not closed
    async_read();
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

void session::async_write(std::vector<char> response)
{
  boost::asio::async_write(socket_, boost::asio::buffer(response),
                           boost::bind(&session::handle_write, this, boost::asio::placeholders::error));
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