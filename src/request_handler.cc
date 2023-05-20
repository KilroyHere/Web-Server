#include "request_handler.h"

EchoRequestHandler::EchoRequestHandler(const std::string &request_uri, NginxConfig &config) {}

StaticRequestHandler::StaticRequestHandler(const std::string &request_uri, NginxConfig &config) : config_(config), file_path(request_uri) {}

BadRequestHandler::BadRequestHandler(const std::string &request_uri, NginxConfig &config) {}

NotFoundRequestHandler::NotFoundRequestHandler(const std::string &request_uri, NginxConfig &config) {}

CrudRequestHandler::CrudRequestHandler(const std::string &request_uri, NginxConfig &config) : config_(config), data_path(request_uri) {}

bool EchoRequestHandler::handle_request(const http::request<http::string_body> http_request, http::response<http::string_body> *http_response)
{
  std::ostringstream oss;
  oss << http_request;
  http_response->result(http::status::ok);
  http_response->version(http_request.version());
  http_response->body() = oss.str();
  http_response->set(http::field::content_type, "text/plain");
  http_response->prepare_payload();
  return true;
}
bool StaticRequestHandler::handle_request(const http::request<http::string_body> http_request, http::response<http::string_body> *http_response)
{
  // read file contents
  std::ifstream file(file_path.c_str(), std::ios::in | std::ios::binary);
  if (file.fail())
  {
    BOOST_LOG_TRIVIAL(info) << "Couldn't open file";
    http_response->result(http::status::not_found);
    http_response->body() = "";
    http_response->prepare_payload();
    return true;
  }
  BOOST_LOG_TRIVIAL(info) << "Reading the file.";
  std::string body = "";

  char c;
  while (file.get(c))
    body += c;

  BOOST_LOG_TRIVIAL(info) << "Got file contents.";

  file.close();

  BOOST_LOG_TRIVIAL(info) << "File closed.";

  std::string extension;
  size_t cursor = file_path.find_last_of(".");
  if (cursor != std::string::npos)
    extension = file_path.substr(cursor + 1);

  http_response->result(http::status::ok);
  http_response->version(http_request.version());
  http_response->body() = body;
  http_response->set(http::field::content_type, extension_to_type(extension));
  http_response->prepare_payload();
  return true;
}

bool BadRequestHandler::handle_request(const http::request<http::string_body> http_request, http::response<http::string_body> *http_response)
{
  http_response->result(http::status::bad_request);
  http_response->body() = "";
  http_response->prepare_payload();
  return true;
}

bool NotFoundRequestHandler::handle_request(const http::request<http::string_body> http_request, http::response<http::string_body> *http_response)
{
  http_response->result(http::status::not_found);
  http_response->body() = "";
  http_response->prepare_payload();
  return true;
}

bool CrudRequestHandler::handle_request(const http::request<http::string_body> http_request, http::response<http::string_body> *http_response) 
{
  if (http_request.method() == http::verb::post) 
  { // POST method
    // Get the next available id
    boost::filesystem::path path(data_path);

    // Create data_path directory if it doesn't already exist
    if (!boost::filesystem::exists(path))
    {
      BOOST_LOG_TRIVIAL(info) << "Creating data_path directory: " << data_path;
      boost::filesystem::create_directories(path);
    }

    int id = 1;
    while (boost::filesystem::exists(path / std::to_string(id)))
    {
      id++;
    }

    // Write data to file
    std::ofstream file((path / std::to_string(id)).string(), std::ios::out | std::ios::binary);
    if (file.fail())
    {
      BOOST_LOG_TRIVIAL(error) << "Couldn't open file";
      http_response->result(http::status::internal_server_error);
      return true;
    }

    BOOST_LOG_TRIVIAL(info) << "Writing POST to file:" << (path / std::to_string(id)).string();

    file << http_request.body();
    file.close();

    // Respond with the new id
    http_response->result(http::status::created);
    http_response->version(http_request.version());
    http_response->body() = "{\"id\": " + std::to_string(id) + "}";
    http_response->set(http::field::content_type, "application/json");
    http_response->prepare_payload();

    return true;

  } 
  else if (http_request.method() == http::verb::get) // GET method
  {
    std::string target = http_request.target().to_string();
    
    int id;
    try 
    {
        // Extract the id from the target
        std::string id_str = target.substr(target.find_last_of("/") + 1);
        id = std::stoi(id_str);
    }
    catch (std::invalid_argument& e)
    {
        BOOST_LOG_TRIVIAL(warning) << "Request does not contain a valid ID: " << target;
        http_response->result(http::status::bad_request);
        http_response->body() = "400: Bad Request. The ID is invalid.";
        http_response->prepare_payload();
        return true;
    }

    boost::filesystem::path path = data_path / boost::filesystem::path(std::to_string(id));    
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (file.fail()) 
    {
      BOOST_LOG_TRIVIAL(warning) << "Attempted to GET ID that does not exist: " << std::to_string(id);
      http_response->result(http::status::not_found);
      http_response->body() = "404 Not Found: The ID does not exist.";
      http_response->prepare_payload();
      return true;
    }

    BOOST_LOG_TRIVIAL(info) << "Reading for GET from file:" << path.string();
    std::string body = "";

    char c;
    while (file.get(c)) body += c;
    file.close();

    http_response->result(http::status::ok);
    http_response->version(http_request.version());
    http_response->body() = body;
    http_response->set(http::field::content_type, "application/json");
    http_response->prepare_payload();
    return true;

  } 
  else
  {
    BOOST_LOG_TRIVIAL(info) << "Client used unsupported HTTP Verb: " << http_request.method_string();
    http_response->result(http::status::method_not_allowed);
    return true;
  }
}