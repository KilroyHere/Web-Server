#include "session_old.h"

session_old::session_old(boost::asio::io_service &io_service, NginxConfig config)
    : socket_(io_service), config_(config), request_handler_(RequestHandler(config))
{
  data_.resize(max_buffer_size);
}

// session::session(boost::asio::io_service &io_service)
//     : socket_(io_service)
// {
//   data_.resize(max_buffer_size);
// }

session_old::~session_old()
{
  socket_.close();
  BOOST_LOG_TRIVIAL(info) << "Run session destructor";
}

tcp::socket &session_old::get_socket()
{
  return socket_;
}

void session_old::async_read()
{
  socket_.async_read_some(boost::asio::buffer(data_),
                          boost::bind(&session_old::handle_read, this,
                                      boost::asio::placeholders::error,
                                      boost::asio::placeholders::bytes_transferred));
}

int session_old::handle_read(const boost::system::error_code &error, size_t bytes_transferred)
{
  std::string log_message = "==================REQUEST===================\n";
  if (!error)
  {
    // LOGGING:
    log_message += "Client: " + boost::lexical_cast<std::string>(socket_.remote_endpoint());
    log_message += "\n===========================================\n";
    for (int i = 0; i < bytes_transferred; i++)
    {
      log_message += data_[i];
    }
    log_message += "\n===============END-OF-REQUEST===============";
    BOOST_LOG_TRIVIAL(info) << log_message;
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
        socket_.close();
        return 0;
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
      BOOST_LOG_TRIVIAL(severity_level::error) << error << ": Error reading from TCP socket";
    }
    socket_.close();
    return 1;
  }
  // Shouldn't reach here
  return 1;
}

void session_old::async_write(std::vector<char> response)
{
  boost::asio::async_write(socket_, boost::asio::buffer(response),
                           boost::bind(&session_old::handle_write, this, boost::asio::placeholders::error));
}

int session_old::handle_write(const boost::system::error_code &error)
{
  if (!error)
  {
    return 0;
  }
  else
  {
    socket_.close();
    return 1;
  }
}
