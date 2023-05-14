#include "request_handler_factory.h"

// Constructor
RequestHandlerFactory::RequestHandlerFactory(NginxConfig config) : parsed_config(config)
{
  set_path_handler_map();
  set_path_root_map();
}

void RequestHandlerFactory::set_path_handler_map()
{
  // Query locations for paths.
  std::vector<std::string> path_query{"location"};
  std::vector<std::string> paths;
  bool root_found = parsed_config.query_config(path_query, paths);
  for (auto path : paths)
  {
    // Look for the root folder for the path
    std::vector<std::string> handler_query{"location", path};
    std::vector<std::string> handlers;
    bool root_found = parsed_config.query_config(handler_query, handlers);

    // only add root to path if root is found
    if (root_found)
    {
      // std::cerr << handlers.at(0) << " " << path << std::endl;
      path_handler_map_[path] = handlers.at(0);
    }
  }
}

// Parses the config file
void RequestHandlerFactory::set_path_root_map()
{

  for (auto path_handler_pair : path_handler_map_)
  {
    // Look for the root folder for the path
    std::vector<std::string> root_query{"location", path_handler_pair.first, path_handler_pair.second, "root"};
    std::vector<std::string> roots;
    bool root_found = parsed_config.query_config(root_query, roots);

    // only add root to path if root is found
    if (root_found)
    {
      // std::cerr << roots.at(0) << " " << path_handler_pair.first << " " << path_handler_pair.second << std::endl;
      path_root_map_[path_handler_pair.first] = roots.at(0);
    }
  }
}

std::unique_ptr<NewRequestHandler> RequestHandlerFactory::createHandler(const http::request<http::string_body> *http_request)
{
  // Find the corresponding handler based on the URI prefix
  std::string request_uri = http_request->target().to_string();
  std::vector<std::string> uri_vector; 

  boost::split(uri_vector, request_uri, boost::is_any_of("/"));
  std::string uri_path = uri_vector.size() >= 2 ? "/" + uri_vector.at(1) : std::string("");
  
  std::cerr << uri_path << std::endl;
  //check if uri_path is empty
  if (uri_path.empty())
  {
    return std::make_unique<NotFoundRequestHandler>(request_uri, parsed_config);
  }
  std::string handler = path_handler_map_.find(uri_path) != path_handler_map_.end() ? path_handler_map_[uri_path] : std::string("");
  std::string root = uri_vector.size() > 2 ? "/" + uri_vector.at(2) : std::string("");
  
  // static handler check
  if ( handler == "StaticRequestHandler" && path_root_map_.find(root) == path_root_map_.end()) // Unique static handlers -- should be in request handler -> static handler
  {
    return std::make_unique<NotFoundRequestHandler>(request_uri, parsed_config);    
  }

  else if (handler == "EchoRequestHandler")
  {
    return std::make_unique<EchoRequestHandler>(request_uri, parsed_config); // Example RequestHandler creation
  }
  else if (handler == "StaticRequestHandler")
  {
    return std::make_unique<StaticRequestHandler>(request_uri, parsed_config); // Example RequestHandler creation
  }
  else
  {
    return std::make_unique<NotFoundRequestHandler>(request_uri, parsed_config); // Default handler if location not found
  }
}