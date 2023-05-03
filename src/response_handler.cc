#include "response_handler.h"


EchoResponseHandler::EchoResponseHandler(Request* request, Response* response)
  : request_(request), response_(response)
{}


FileResponseHandler::FileResponseHandler(Request* request, Response* response, std::string filepath) 
    : request_(request), response_(response), path_(filepath)
{
  //check filepath  
  std::cout<<"filepath: "<<filepath<<"\n";    
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


void FileResponseHandler::set_response_fields()
{    
    response_->set_file_response(200, path_);
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
