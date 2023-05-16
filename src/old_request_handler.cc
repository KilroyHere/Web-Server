#include "request_handler.h"

RequestHandler::RequestHandler(NginxConfig config) : state_(READING_HEADER), connection_close_(false), response_code_(200), body_read_(0), config_(config)
{
  set_path_root_map();
}

void RequestHandler::set_path_root_map()
{
  // Query server locations for paths.
  std::vector<std::string> path_query{"server", "location"};
  std::vector<std::string> paths;
  bool root_found = config_.query_config(path_query, paths);
  for (auto path : paths)
  {
    // Look for the root folder for the path
    std::vector<std::string> root_query{"server", "location", path, "root"};
    std::vector<std::string> roots;
    bool root_found = config_.query_config(root_query, roots);

    // only add root to path if root is found
    if (root_found)
    {
      path_root_map_[path] = roots.at(0);
    }
  }
}

int RequestHandler::handle_request(std::vector<char> data, size_t bytes_transferred)
{
  // If currenlty reading the header
  if (state_ == READING_HEADER)
  {
    RequestParser::result_type result = parser_.parse(request_, data, bytes_transferred);
    // If read the header successfully and completely
    if (result == RequestParser::good)
    {
      // LOGGING:
      BOOST_LOG_TRIVIAL(info) << "===============GOOD REQUEST!!===============";
      response_code_ = 200;
      int read_from = parser_.read_from_;
      request_.set_headers_map();
      parser_.reset();

      // If connection:close, end connection
      if (request_.headers_map.find("connection") != request_.headers_map.end())
      {
        if (request_.headers_map["connection"] == "close")
        {
          connection_close_ = true;
        }
      }

      //  Check for content-length and parse body
      if (request_.headers_map.find("content-length") != request_.headers_map.end())
      {
        // Switch handler state
        state_ = READING_BODY;
        return read_body(data, read_from, bytes_transferred);
      }
      return 1;
    }
    // If the HTTP request is malformed
    else if (result == RequestParser::bad)
    {
      // LOGGING:
      BOOST_LOG_TRIVIAL(info) << "===============BAD REQUEST!!===============";
      response_code_ = 400;
      connection_close_ = true;
      return 2;
    }
    // If the header hasn't been read completely
    else
    {
      return 0;
    }
  }
  // If currently reading the body
  else
  {
    int read_from = parser_.read_from_;
    return read_body(data, read_from, bytes_transferred);
  }
}

void RequestHandler::reset()
{
  // Ready for a new request in the same session
  state_ = READING_HEADER;
  body_read_ = 0;
  request_.purge_request();
  response_.purge_response();
}

int RequestHandler::read_body(std::vector<char> data, int read_from, int bytes_transferred)
{
  int content_length = stoi(request_.headers_map["content-length"]);

  for (int i = read_from; i < bytes_transferred && body_read_ < content_length; i++)
  {
    request_.request_body.push_back(data.at(i));
    body_read_ += 1;
  }
  if (body_read_ == content_length)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

void RequestHandler::set_response()
{
  std::unique_ptr<ResponseHandler> response_handler;

  if (response_code_ == 200)
  {
    // Extract URI path components.
    std::string uri = request_.uri;
    // Holds the complete path seperated by "/"
    std::vector<std::string> path_vector;
    boost::split(path_vector, uri, boost::is_any_of("/"));
    // The top level path
    std::string uri_path = path_vector.size() >= 2 ? "/" + path_vector.at(1) : std::string("");

    // Determine response handler based on URI path.
    if (uri_path.empty())
    {
      connection_close_ = true;
      response_handler = std::make_unique<NotFoundResponseHandler>(&request_, &response_);
    }
    else if (uri_path == "/echo")
    {
      response_handler = std::make_unique<EchoResponseHandler>(&request_, &response_);
    }
    else if (path_root_map_.find(uri_path) != path_root_map_.end() && path_vector.size() == 3)
    {
      BOOST_LOG_TRIVIAL(info) << "Path Exists: " << uri_path;
      std::string root_folder = path_root_map_[uri_path];

      // Setting filepath to ../<folder>/<file_name>
      std::string filepath = "../static" + root_folder + "/" + path_vector.at(2);

      // Check if file exists using relative file format server/static/folder/filename

      boost::filesystem::path boost_path(filepath);
      if (!boost::filesystem::exists(filepath) || !boost::filesystem::is_regular_file(filepath))
      {
        BOOST_LOG_TRIVIAL(info) << "File does not exist in this directory: "
                                << ".." << root_folder;
        connection_close_ = true;
        response_handler = std::make_unique<NotFoundResponseHandler>(&request_, &response_);
      }
      else
      {
        response_handler = std::make_unique<FileResponseHandler>(&request_, &response_, filepath);
      }
    }
    else
    {
      connection_close_ = true;
      response_handler = std::make_unique<NotFoundResponseHandler>(&request_, &response_);
    }
  }
  else
  {
    connection_close_ = true;
    response_handler = std::make_unique<BadRequestResponseHandler>(&request_, &response_);
  }

  // Set response fields based on selected handler.
  response_handler->set_response_fields();
}

std::vector<char> RequestHandler::get_response()
{
  // Echo response
  return response_.to_buffer();
}

bool RequestHandler::connection_close()
{
  return connection_close_;
}