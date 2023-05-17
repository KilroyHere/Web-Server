# Git Gud  
Welcome to our project! This README document provides an overview of the source code, instructions on how to build, test, and run the code, as well as guidelines on how to add a new request handler. We aim to make it easy for contributors, even those unfamiliar with the codebase, to get started and make meaningful contributions.
## Source Code Overview 
### Main 
(server_main.cc)
- The server_main.cc is responsible for creating an http server instance and reading in the config file and sending a parsed config to the server
### HTTPserver 
(http_server.cc http_server.h)
 - The HTTPserver class is responsible for setting up new connections with new clients and creating sessions upon new connections
 - Just one instance of the server runs through out.
 - The server is also responsible for querying the parser to set up the reading the network socket at correct port.
### Session 
(session.cc session.h)
- The session class is responsible for handling i/o with a specific client. 
- For every client the session listens at the socket for requests, parses the requests and sends appropriate responses over the HTTP 1.1 protocol
- session uses the RequestHandlerFactory to create the appropriate RequestHandler according to the response
### RequestHandlerFactory 
(request_handler_factory.cc request_handler_factory.h)
- The factory creates a RequestHandler, looking at the parsed request and then matching the request location to the correct Handler.
- A handler is mapped to a location as follows in the config file:
``` nginx
location /static StaticRequestHandler {
  root ./folder1; # supports relative paths
}
```
### RequestHandler 
(request_handler.cc request_handler.h)
- The files request_handler.cc and request_handler.h are responsible for creating a response to the parsed request
- They are passed in the Request URI and the parsed config file for complete autonomy over the response creation
-  There are currently 4 type of handlers:
    -  EchoRequestHandler
        -  Sends back an echo response
    -  StaticRequestHandler
        -  Sends the requested static file
    -  BadRequestHander
        -  Sends a 400 error respose for malformed requests 
    -  NotFoundRequestHandler
        -  Sends a 404 error for an unset location or a non existing file

### MimeTypes
(mime_types.cc mime_types.h)
- has one main function that is responsible for returning the correct corresponding content_type, given the type of file.
    - extension_to_type
        - Takes in a string of the type of file it is dealing with and returns the corresponding content_type tag for the response object.


### Code Structure
``` 
├── CMakeLists.txt
├── README.md
├── cmake
│   └── CodeCoverageReportConfig.cmake
├── config
│   ├── config_parser.cc
│   ├── deploy_config
│   └── server_config
├── docker
│   ├── Dockerfile
│   ├── base.Dockerfile
│   ├── cloudbuild.yaml
│   └── coverage.Dockerfile
├── folder1
│   ├── index.html
│   ├── mime_jpeg_test.jpeg
│   ├── mime_txt_test.txt
│   └── ziptest.pdf
├── folder2
│   ├── mime_pdf_test.pdf
│   ├── mime_png_test.png
│   ├── mime_zip_test.zip
│   └── ziptest.pdf
├── folder3
│   └── mime_gif_test.gif
├── include
│   ├── config_parser.h
│   ├── http_request.h
│   ├── http_server.h
│   ├── logging.h
│   ├── mime_types.h
│   ├── request_handler.h
│   ├── request_handler_factory.h
│   ├── request_parser.h
│   └── session.h
├── make_script.sh
├── src
│   ├── http_request.cc
│   ├── http_server.cc
│   ├── logging.cc
│   ├── mime_types.cc
│   ├── request_handler.cc
│   ├── request_handler_factory.cc
│   ├── request_parser.cc
│   ├── server_main.cc
│   └── session.cc
└── tests
    ├── config_parser_test.cc
    ├── config_parser_tests
    │   ├── comment_config
    │   ├── correct_port_config
    │   ├── deploy_config
    │   ├── empty_config
    │   ├── empty_port_config
    │   ├── empty_server_config
    │   ├── example_config
    │   ├── extra_left_brace_config
    │   ├── extra_right_brace_config
    │   ├── incorrect_port_config
    │   ├── invalid_location_config
    │   ├── invalid_port_config
    │   ├── invalid_quotation_config
    │   ├── nested_config
    │   ├── new_server_config
    │   ├── no_path_config
    │   ├── no_semi_colon_config
    │   ├── nonnumber_port_config
    │   └── several_server_config
    ├── integration_tests
    │   ├── curl_expected.txt
    │   ├── logs
    │   │   └── server_log_0.log
    │   ├── nc_expected.txt
    │   └── test.sh
    ├── mime_tests
    │   ├── index.html
    │   ├── mime_gif_test.gif
    │   ├── mime_jpeg_test.jpeg
    │   ├── mime_pdf_test.pdf
    │   ├── mime_png_test.png
    │   ├── mime_txt_test.txt
    │   └── mime_zip_test.jpeg.zip
    ├── proxy_config
    ├── request_handler_test.cc
    ├── request_handler_tests
    │   ├── good_request
    │   ├── good_request_response
    │   ├── good_request_with_response_index_html
    │   ├── good_request_with_response_mime_jpeg_test
    │   ├── good_request_with_response_mime_pdf_test
    │   ├── good_request_with_response_mime_png_test
    │   ├── good_request_with_response_mime_txt_test
    │   ├── good_request_with_response_mime_zip_test
    │   ├── good_request_with_response_static1_zip_test
    │   ├── good_request_with_response_static2_mime_gif_test
    │   └── good_request_with_response_ziptest_pdf
    ├── request_parser_test.cc
    ├── request_parser_tests
    │   ├── bad_header_request_1
    │   ├── bad_header_request_2
    │   ├── bad_header_request_3
    │   ├── bad_http_request_1
    │   ├── bad_http_request_2
    │   ├── bad_http_request_3
    │   ├── bad_http_request_4
    │   ├── bad_http_request_5
    │   ├── bad_http_version_request_1
    │   ├── bad_http_version_request_2
    │   ├── bad_http_version_request_3
    │   ├── bad_http_version_request_4
    │   ├── bad_method_request
    │   ├── bad_method_start_request
    │   ├── good_http_version_request_1
    │   ├── good_http_version_request_2
    │   ├── good_lws_request_1
    │   ├── good_lws_request_2
    │   ├── good_lws_request_3
    │   ├── good_request
    │   └── indeterminate_request
    └── session_test.cc
```


## Building the Code

### To build the code, follow these steps:
- Navigate to the project directory: `cd <project-directory>`
- Create a build directory: `mkdir build && cd build `
- Start the build: `cmake .. && make`

### To build the coverage, follow these steps:
- Navigate to the project directory: `cd <project-directory>`
- Create a build directory: `mkdir build_coverage && cd build_coverage `
- Start the build: `cmake -DCMAKE_BUILD_TYPE=Coverage .. && make coverage`

## Running the server
### To run the server, follow these steps:
- Run the server from the build directory: `./bin/server ../config/server_config`

## Adding a Request Handler 
To add a new request handler to the project, follow these steps:
* Update the config file to set up the location at which the handler should work:
```nginx
port 8080;  # port my server listens on

location /echo EchoRequestHandler  {  # no arguments
}

location /static  StaticRequestHandler  {
root ./folder1;  # supports relative paths
}

# Your Custom Handler:
location /<location> <YourRequestHandler> {
root <relative_folder_path>;
}
```
* In the *request_handler.h*  and *request_handler.cc* files located in the include and src directory respectively: 
	*  Create a new class that inherits from the RequestHandler base class. 
*  	Implement the `handle_request()` method in the derived class to provide the logic for handling the request and generating the response. 
	* Add the necessary constructor and member variables to the derived class to support any configuration or dependencies. 
* In the *request_handler_factory.cc* file, update the `create_handler()` function to add a corresponding if statement to add that handler as a possible request.
* Document the new request handler in the code and provide relevant comments for better understanding. 

### Example Request Handler 
Here is an example of an existing request handler implementation:
 ``` cpp
 class ImageRequestHandler: public RequestHandler {
public:
    ImageRequestHandler(const std::string& path, const NginxConfig& config) {
        // Retrieve any necessary information from the config file
        // Example: Parse the config to get specific settings for the ImageRequestHandler
    }

    bool handle_request(const request<string_body> request, response<string_body>* response) override {
        // Handle the request logic based on the parsed request
        // Generate the appropriate response

        // Example: Echo the request body in the response
        response->result(http::status::ok);
        response->version(request.version());
        response->set(http::field::content_type, extension_to_type("txt"));
        response->body() = request.body();
        response->prepare_payload();

        return true; // Request handled successfully
    }
};
 ```
 In this example, the ImageRequestHandler is a request handler that echoes the request body in the response. It takes in a path and a config object as constructor parameters, allowing customization based on the configuration. The `handle_request()` method implements the logic to generate the appropriate response. As shown from this example, we can set multiple fields in the response such as the version, body, status, and content_type. Remember after setting the fields to provide `prepare_payload()` in order to prepare the response. Also notice that we can use `extension_to_type()` where it takes in a parameter to determine the correct content_type. Refer to the mime_types class. 
 
### Automate Builds
- A shell script for automating builds for make test and build_coverage. To learn more about the automating builds, refer to the help flag `-h` for more information on how to automate builds. Do keep in mind you can group together flags, e.g. `-tc`, to do build and build_coverage. Remember to be in the same directory as the file to run the command below.
```
$ ./make_script -h 
```

### Documentation 
For more detailed documentation, please refer to the inline comments in the source code files. They provide additional explanations, guidelines, and examples to help you understand and work with the codebase effectively.

License This project is licensed under the MIT License. You are free to use, modify, and distribute the code in accordance with the terms of the license.

Happy coding!