#ifndef RESPONSE_HANDLER_H
#define RESPONSE_HANDLER_H

#include <iostream>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/log/trivial.hpp>
#include "http_request.h"
#include "http_response.h"
#include "mime_types.h"

class ResponseHandler
{
public:
  virtual void set_response_fields() = 0;
  // Make the class polymorphic with a virtual destructor
  virtual ~ResponseHandler(){};
};

class EchoResponseHandler : public ResponseHandler
{
public:
  EchoResponseHandler(Request *request, Response *response);
  void set_response_fields() override;

private:
  Response *response_;
  Request *request_;
};

class FileResponseHandler : public ResponseHandler
{
public:
  FileResponseHandler(Request *request, Response *response);
  FileResponseHandler(Request *request, Response *response, std::string filepath);

  void set_response_fields() override;

private:
  Response *response_;
  Request *request_;
  std::string path_;
};

class BadRequestResponseHandler : public ResponseHandler
{
public:
  BadRequestResponseHandler(Request *request, Response *response);
  void set_response_fields() override;

private:
  Response *response_;
  Request *request_;
};

class NotFoundResponseHandler : public ResponseHandler
{
public:
  NotFoundResponseHandler(Request *request, Response *response);
  void set_response_fields() override;
  // Response get_response() override;

private:
  Response *response_;
  Request *request_;
};

#endif // RESPONSE_HANDLER_H