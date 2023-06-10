#include "request_handler.h"

EchoRequestHandler::EchoRequestHandler(const std::string &request_uri, NginxConfig &config) {}

StaticRequestHandler::StaticRequestHandler(const std::string &request_uri, NginxConfig &config) : config_(config), file_path(request_uri) {}

BadRequestHandler::BadRequestHandler(const std::string &request_uri, NginxConfig &config) {}

NotFoundRequestHandler::NotFoundRequestHandler(const std::string &request_uri, NginxConfig &config) {}

CrudRequestHandler::CrudRequestHandler(const std::string &request_uri, NginxConfig &config) : config_(config), data_path(request_uri) {}

HealthRequestHandler::HealthRequestHandler(const std::string &request_uri, NginxConfig &config) {}

SleepRequestHandler::SleepRequestHandler(const std::string &request_uri, NginxConfig &config) {}

AuthenticationRequestHandler::AuthenticationRequestHandler(const std::string &request_uri, NginxConfig &config) : config_(config), data_path(request_uri) {}

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
  std::lock_guard<std::mutex> lock(mutex_file);
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
  // Checking length of URI
  std::string complete_uri = http_request.target().to_string();

  int length = 0;
  for (int i = 0; i < complete_uri.length(); i++)
  {
    if (complete_uri[i] == '/')
    {
      length++;
    }
  }

  if (complete_uri[complete_uri.length() - 1] == '/' && http_request.method() != http::verb::post)
  {
    BOOST_LOG_TRIVIAL(warning) << "Request does not contain a valid URI";
    http_response->result(http::status::bad_request);
    http_response->body() = "400: Bad Request. The ID is invalid.";
    http_response->prepare_payload();
    return true;
  }

  // Incorrect length of URI, malformed request
  if (length != 2 && length != 3)
  {
    BOOST_LOG_TRIVIAL(warning) << "Request does not contain a valid URI";
    http_response->result(http::status::bad_request);
    http_response->body() = "400: Bad Request. The ID is invalid.";
    http_response->prepare_payload();
    return true;
  }

  if (http_request.method() == http::verb::post)
  { // POST method
    // Get the next available id
    boost::filesystem::path path(data_path);

    // Create data_path directory if it doesn't already exist
    std::lock_guard<std::mutex> lock(mutex_file);
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
    std::regex regex_expr("[0-9]+");
    std::string id_str = target.substr(target.find_last_of("/") + 1);
    if (std::regex_match(id_str, regex_expr))
    {
      BOOST_LOG_TRIVIAL(info) << "Returning single json object in GET request";
      int id;
      try
      {
        // Extract the id from the target
        std::string id_str = target.substr(target.find_last_of("/") + 1);
        id = std::stoi(id_str);
      }
      catch (std::invalid_argument &e)
      {
        BOOST_LOG_TRIVIAL(warning) << "Request does not contain a valid ID: " << target;
        http_response->result(http::status::bad_request);
        http_response->body() = "400: Bad Request. The ID is invalid.";
        http_response->prepare_payload();
        return true;
      }
      std::lock_guard<std::mutex> lock(mutex_file);
      boost::filesystem::path path = data_path / boost::filesystem::path(std::to_string(id));
      std::ifstream file(path.string(), std::ios::in | std::ios::binary);
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
      while (file.get(c))
        body += c;
      file.close();

      http_response->result(http::status::ok);
      http_response->version(http_request.version());
      http_response->body() = body;
      http_response->set(http::field::content_type, "application/json");
      http_response->prepare_payload();
    }
    else
    {
      BOOST_LOG_TRIVIAL(info) << "Returning list in GET request";
      std::string body = "[";

      boost::filesystem::path path(data_path);
      if (!boost::filesystem::exists(path) || boost::filesystem::is_regular_file(path))
      {
        BOOST_LOG_TRIVIAL(warning) << "Attempted to GET directory that does not exist: " << data_path;
        http_response->result(http::status::not_found);
        http_response->body() = "404 Not Found: The directory does not exist.";
        http_response->prepare_payload();
        return true;
      }
      boost::filesystem::directory_iterator it(path);
      boost::filesystem::path file_path;
      while (it != boost::filesystem::directory_iterator{})
      {
        file_path = it->path();
        BOOST_LOG_TRIVIAL(info) << "Adding file: " << file_path.string() << " to list of files";
        if (boost::filesystem::is_regular_file(file_path))
        {
          target = file_path.string();
          std::string id_str = target.substr(target.find_last_of("/") + 1);
          body += id_str + ",";
        }
        it++;
      }
      body.pop_back();
      body += "]";

      http_response->result(http::status::ok);
      http_response->version(http_request.version());
      http_response->body() = body;
      http_response->set(http::field::content_type, "application/json");
      http_response->prepare_payload();
    }
    return true;
  }
  // PUT method
  else if (http_request.method() == http::verb::put)
  {
    std::string target = http_request.target().to_string();

    int id;
    try
    {
      // Extract the id from the target
      std::string id_str = target.substr(target.find_last_of("/") + 1);
      id = std::stoi(id_str);
    }
    catch (std::invalid_argument &e)
    {
      BOOST_LOG_TRIVIAL(warning) << "Request does not contain a valid ID: " << target;
      http_response->result(http::status::bad_request);
      http_response->body() = "400: Bad Request. The ID is invalid.";
      http_response->prepare_payload();
      return true;
    }

    boost::filesystem::path path = data_path / boost::filesystem::path(std::to_string(id));
    boost::filesystem::path dir = path.parent_path();

    http::status status = http::status::ok;

    if (!boost::filesystem::exists(path))
    {
      BOOST_LOG_TRIVIAL(info) << "File being PUT will be created";
      status = http::status::created;
    }

    if (!boost::filesystem::exists(dir))
    {
      BOOST_LOG_TRIVIAL(info) << "Creating data_path directory: " << data_path;
      boost::filesystem::create_directories(dir);
    }
    std::lock_guard<std::mutex> lock(mutex_file);
    std::ofstream file(path.string(), std::ios::out | std::ios::binary | std::ios::trunc);
    if (file.fail())
    {
      BOOST_LOG_TRIVIAL(error) << "Couldn't open file";
      http_response->result(http::status::internal_server_error);
      return true;
    }

    BOOST_LOG_TRIVIAL(info) << "Writing PUT to file:" << path;

    file << http_request.body();
    file.close();

    http_response->result(status);
    http_response->version(http_request.version());
    http_response->prepare_payload();

    return true;
  }
  // DELETE method
  else if (http_request.method() == http::verb::delete_)
  {
    // Get full file path
    std::string target = http_request.target().to_string();

    int id;
    try
    {
      // Extract the id from the target
      std::string id_str = target.substr(target.find_last_of("/") + 1);
      id = std::stoi(id_str);
    }
    catch (std::invalid_argument &e)
    {
      BOOST_LOG_TRIVIAL(warning) << "Request does not contain a valid ID: " << target;
      http_response->result(http::status::bad_request);
      http_response->body() = "400: Bad Request. The ID is invalid.";
      http_response->prepare_payload();
      return true;
    }

    boost::filesystem::path path = data_path / boost::filesystem::path(std::to_string(id));

    // Decided on 204 HTTP error for attempting to delete non-existent or already deleted files.
    if (!boost::filesystem::exists(path))
    {
      BOOST_LOG_TRIVIAL(info) << "Attempted to DELETE a file " << path.string() << " that does not exist";
      http_response->result(http::status::no_content);
      // the no content response does not accept an http body
      http_response->body() = "";
      http_response->prepare_payload();
      return true;
    }

    // Delete file
    BOOST_LOG_TRIVIAL(info) << "Deleting file " << path.string();
    boost::system::error_code del_ec;
    boost::filesystem::remove_all(path, del_ec);
    if (del_ec)
    {
      BOOST_LOG_TRIVIAL(error) << "Couldn't delete file";
      http_response->result(http::status::internal_server_error);
      return true;
    }

    // Respond with success message if file successfully deleted
    http_response->result(http::status::ok);
    http_response->version(http_request.version());
    http_response->body() = "File " + target + " deleted successfully.";
    http_response->set(http::field::content_type, "text/HTML");
    http_response->prepare_payload();

    return true;
  }
  // Preflight OPTIONS Check
  else if (http_request.method() == http::verb::options)
  {
    http_response->result(http::status::no_content);
    http_response->version(http_request.version());
    http_response->set(http::field::access_control_allow_origin, "*");
    http_response->set(http::field::access_control_allow_methods, "GET, POST, PUT, DELETE");
    http_response->set(http::field::access_control_max_age, "2147483647");
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

bool HealthRequestHandler::handle_request(const http::request<http::string_body> http_request, http::response<http::string_body> *http_response)
{
  std::ostringstream oss;
  oss << "OK";
  http_response->result(http::status::ok);
  http_response->version(http_request.version());
  http_response->body() = oss.str();
  http_response->set(http::field::content_type, "text/plain");
  http_response->prepare_payload();
  return true;
}

bool SleepRequestHandler::handle_request(const http::request<http::string_body> http_request, http::response<http::string_body> *http_response)
{
  BOOST_LOG_TRIVIAL(info) << "sleeping for 1 seconds";
  sleep(1);
  BOOST_LOG_TRIVIAL(info) << "slept for 1 seconds";
  http_response->result(http::status::ok);
  http_response->body() = "";
  http_response->prepare_payload();
  return true;
}

bool AuthenticationRequestHandler::handle_request(const http::request<http::string_body> http_request, http::response<http::string_body> *http_response)
{

  // GET for login.html
  if (http_request.method() == http::verb::get)
  {

    std::vector<std::string> tokens;
    boost::split(tokens, data_path, boost::is_any_of("/"));

    BOOST_LOG_TRIVIAL(info) << data_path;
    BOOST_LOG_TRIVIAL(info) << tokens.size();

    std::vector<std::string> values;
    // Handle token size for deployment or local development
    if (tokens.size() == 5) 
    {
      boost::split(values, tokens[4], boost::is_any_of("="));
    }
    else if (tokens.size() == 3) 
    {
      boost::split(values, tokens[2], boost::is_any_of("="));
    }
    else 
    {
      BOOST_LOG_TRIVIAL(info) << "Malformed Request";
      http_response->result(http::status::not_found);
      http_response->prepare_payload();
      return true;
    }

    BOOST_LOG_TRIVIAL(info) << values.size();

    if (values.size() == 2)
    {
      BOOST_LOG_TRIVIAL(info) << "Well formed Request received";
      std::string password = values[1];
      std::string username = values[0];
      std::string new_path = "";
      if (tokens.size() == 5)
      {
        new_path = tokens[0] + "/" + tokens[1] + "/" + tokens[2] + "/" + tokens[3] + "/" + username;
      }
      else 
      {
        new_path = tokens[0] + "/" + tokens[1] + "/" + username;
      }

      size_t hashed_password = boost::hash<std::string>{}(password);

      std::string uri = "/rplacedata/" + username + "/" + "1";

      CrudRequestHandler crh(new_path, config_);
      http::request<http::string_body> req_1{http::verb::get, uri, 10};

      http::response<http::string_body> res_1;
      crh.handle_request(req_1, &res_1);

      // found file
      if (res_1.result() == http::status::not_found)
      {
        BOOST_LOG_TRIVIAL(info) << "Couldn't find file";
        http_response->result(http::status::not_found);
        http_response->prepare_payload();
        return true;
      }
      if (res_1.result() == http::status::ok)
      {
        std::stringstream ss2;
        ss2 << res_1.body();
        std::string body2 = ss2.str();

        // correct password
        if (body2 == std::to_string(hashed_password))
        {
          http_response->result(http::status::ok);
          http_response->body() = "{\"hashed_password\": \"" + std::to_string(hashed_password) + "\"}";
          http_response->prepare_payload();
          return true;
        }
      }
    }
    // incorrect password
    BOOST_LOG_TRIVIAL(info) << "Incorrect Password";
    http_response->result(http::status::not_found);
    http_response->prepare_payload();
    return true;
  }

  // POST for signup.html
  else if (http_request.method() == http::verb::post)
  {

    std::stringstream ss;
    ss << http_request.body();
    std::string body = ss.str();

    std::vector<std::string> tokens;
    boost::split(tokens, body, boost::is_any_of("&"));

    // username
    std::vector<std::string> username_tokens;
    boost::split(username_tokens, tokens[0], boost::is_any_of("="));
    std::string username = username_tokens[1];

    std::vector<std::string> password_tokens;
    boost::split(password_tokens, tokens[1], boost::is_any_of("="));
    std::string password = password_tokens[1];
    size_t hashed_password = boost::hash<std::string>{}(password);

    std::string uri = "/rplacedata/" + username;

    CrudRequestHandler crh(data_path, config_);
    http::request<http::string_body> req_1{http::verb::get, uri, 10};

    http::response<http::string_body> res_1;
    crh.handle_request(req_1, &res_1);

    // user doesn't exist
    if (res_1.result() == http::status::not_found)
    {
      http::request<http::string_body> req_2{http::verb::post, uri, 10};
      http::response<http::string_body> res_2;
      req_2.body() = std::to_string(hashed_password);
      crh.handle_request(req_2, &res_2);

      http_response->result(http::status::ok);
      http_response->prepare_payload();
      return true;
    }

    // user does exist
    else
    {
      http_response->result(http::status::not_found);
      http_response->prepare_payload();
      return true;
    }
  }

  // PUT for index.html
  else if (http_request.method() == http::verb::put)
  {

    std::vector<std::string> tokens;
    boost::split(tokens, data_path, boost::is_any_of("/"));

    BOOST_LOG_TRIVIAL(info) << data_path;
    BOOST_LOG_TRIVIAL(info) << tokens.size();

    std::vector<std::string> values;
    // Handle token size for deployment or local development
    if (tokens.size() == 5) 
    {
      boost::split(values, tokens[4], boost::is_any_of("="));
    }
    else if (tokens.size() == 3) 
    {
      boost::split(values, tokens[2], boost::is_any_of("="));
    }
    else 
    {
      BOOST_LOG_TRIVIAL(info) << "Malformed Request";
      http_response->result(http::status::not_found);
      http_response->prepare_payload();
      return true;
    }

    BOOST_LOG_TRIVIAL(info) << values.size();

    if (values.size() == 2)
    {

      std::string password = values[1];
      std::string username = values[0];
      std::string new_path = "";

      if (tokens.size() == 5)
      {
        new_path = tokens[0] + "/" + tokens[1] + "/" + tokens[2] + "/" + tokens[3] + "/" + username;
      }
      else 
      {
        new_path = tokens[0] + "/" + tokens[1] + "/" + username;
      }

      size_t hashed_password = boost::hash<std::string>{}(password);

      std::string uri = "/rplacedata/" + username + "/" + "1";

      CrudRequestHandler crh(new_path, config_);
      http::request<http::string_body> req_1{http::verb::get, uri, 10};

      http::response<http::string_body> res_1;
      crh.handle_request(req_1, &res_1);

      // found file
      if (res_1.result() == http::status::ok)
      {
        std::stringstream ss2;
        ss2 << res_1.body();
        std::string body2 = ss2.str();

        // correct password
        if (body2 == password)
        {
          http_response->result(http::status::ok);
          http_response->body() = "{\"hashed_password\": " + std::to_string(hashed_password) + "}";
          http_response->prepare_payload();
          return true;
        }
      }
    }
    // incorrect password
    http_response->result(http::status::not_found);
    http_response->prepare_payload();
    return true;
  }

  return true;
}

std::string EchoRequestHandler::get_name()
{
  return "EchoRequestHandler";
}

std::string StaticRequestHandler::get_name()
{
  return "StaticRequestHandler";
}

std::string BadRequestHandler::get_name()
{
  return "BadRequestHandler";
}

std::string NotFoundRequestHandler::get_name()
{
  return "NotFoundRequestHandler";
}

std::string CrudRequestHandler::get_name()
{
  return "CrudRequestHandler";
}

std::string HealthRequestHandler::get_name()
{
  return "HealthRequestHandler";
}

std::string SleepRequestHandler::get_name()
{
  return "SleepRequestHandler";
}

std::string AuthenticationRequestHandler::get_name()
{
  return "AuthenticationRequestHandler";
}