#include "response_handler.h"


EchoResponseHandler::EchoResponseHandler(Request* request, Response* response)
  : request_(request), response_(response)
{}

// TODO:
// FileResponseHandler::FileResponseHandler(const Request request, std::shared_ptr<Response> response)
//   : request_(request), response_(response)
// {}

FileResponseHandler::FileResponseHandler(Request* request, Response* response, std::string filepath, std::string root) 
    : request_(request), response_(response), path(filepath), root_(root)
{
    
}




BadRequestResponseHandler::BadRequestResponseHandler(Request* request, Response* response)
  : request_(request), response_(response)
{}

NotFoundResponseHandler::NotFoundResponseHandler(Request* request, Response* response)
  : request_(request), response_(response)
{}



void EchoResponseHandler::set_response_fields() 
{
    std::string response_body = request_->request_headers + request_->request_body;
    response_->set_echo_response(200, response_body);
  }

// TODO: set_response_fields() function
void FileResponseHandler::set_response_fields()
{
    std::string response_body = "";
    response_->set_echo_response(200, response_body);
}



void BadRequestResponseHandler::set_response_fields() 
{
    std::string response_body = "";
    response_->set_echo_response(400  , response_body);
  }

void NotFoundResponseHandler::set_response_fields() 
{
    std::string response_body = "";
    response_->set_echo_response(404  , response_body);
}
