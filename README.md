# Git Gud  
Welcome to our project! This README document provides an overview of the source code, instructions on how to build, test, and run the code, as well as guidelines on how to add a new request handler. We aim to make it easy for contributors, even those unfamiliar with the codebase, to get started and make meaningful contributions.

## Source Code Overview 
### Code Structure
```
├── CMakeLists.txt
├── cmake
│   └── CodeCoverageReportConfig.cmake
├── config
│   ├── config_parser.cc
│   ├── deploy_config
│   └── server_config
├── docker
│   ├── Dockerfile
│   ├── base.Dockerfile
│   ├── cloudbuild.yaml
│   └── coverage.Dockerfile
├── folder1
│   ├── index.html
│   ├── mime_jpeg_test.jpeg
│   ├── mime_txt_test.txt
│   └── ziptest.pdf
├── folder2
│   ├── mime_pdf_test.pdf
│   ├── mime_png_test.png
│   ├── mime_zip_test.zip
│   └── ziptest.pdf
├── folder3
│   └── mime_gif_test.gif
├── include
│   ├── config_parser.h
│   ├── http_request.h
│   ├── http_server.h
│   ├── logging.h
│   ├── mime_types.h
│   ├── request_handler.h
│   ├── request_handler_factory.h
│   ├── request_parser.h
│   └── session.h
├── make_script.sh
├── src
│   ├── http_request.cc
│   ├── http_server.cc
│   ├── logging.cc
│   ├── mime_types.cc
│   ├── request_handler.cc
│   ├── request_handler_factory.cc
│   ├── request_parser.cc
│   ├── server_main.cc
│   └── session.cc
└── tests
    ├── config_parser_test.cc
    ├── config_parser_tests
    │   ├── comment_config
    │   ├── correct_port_config
    │   ├── deploy_config
    │   ├── empty_config
    │   ├── empty_port_config
    │   ├── empty_server_config
    │   ├── example_config
    │   ├── extra_left_brace_config
    │   ├── extra_right_brace_config
    │   ├── incorrect_port_config
    │   ├── invalid_location_config
    │   ├── invalid_port_config
    │   ├── invalid_quotation_config
    │   ├── nested_config
    │   ├── new_server_config
    │   ├── no_path_config
    │   ├── no_semi_colon_config
    │   ├── nonnumber_port_config
    │   └── several_server_config
    ├── integration_tests
    │   ├── curl_expected.txt
    │   ├── logs
    │   │   └── server_log_0.log
    │   ├── nc_expected.txt
    │   └── test.sh
    ├── mime_tests
    │   ├── index.html
    │   ├── mime_gif_test.gif
    │   ├── mime_jpeg_test.jpeg
    │   ├── mime_pdf_test.pdf
    │   ├── mime_png_test.png
    │   ├── mime_txt_test.txt
    │   └── mime_zip_test.jpeg.zip
    ├── proxy_config
    ├── request_handler_test.cc
    ├── request_handler_tests
    │   ├── bad_header_request_1
    │   ├── bad_header_request_2
    │   ├── bad_header_request_3
    │   ├── bad_http_request_1
    │   ├── bad_http_request_2
    │   ├── bad_http_request_3
    │   ├── bad_http_request_4
    │   ├── bad_http_request_5
    │   ├── bad_http_version_request_1
    │   ├── bad_http_version_request_2
    │   ├── bad_http_version_request_3
    │   ├── bad_http_version_request_4
    │   ├── bad_method_request
    │   ├── bad_method_start_request
    │   ├── bad_request_response
    │   ├── bad_static_request
    │   ├── bad_static_request_response
    │   ├── good_http_version_request_1
    │   ├── good_http_version_request_2
    │   ├── good_lws_request_1
    │   ├── good_lws_request_2
    │   ├── good_lws_request_3
    │   ├── good_request
    │   ├── good_request_response
    │   ├── good_request_with_bad_body
    │   ├── good_request_with_close_connection
    │   ├── good_request_with_good_body
    │   ├── good_request_with_open_connection
    │   ├── good_request_with_response_index_html
    │   ├── good_request_with_response_mime_jpeg_test
    │   ├── good_static_request
    │   ├── good_static_request_response
    │   └── indeterminate_request
    ├── response_handler_test.cc
    └── session_test.cc

```


## Building the Code

### To build the code, follow these steps:
- Navigate to the project directory: `cd <project-directory>`
- Create a build directory: `mkdir build && cd build `
- Start the build: `cmake .. && make`

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
location /<location> <YourRequestHandler {
root <relative_folder_path>;
}
```
* In the *request_handler.h*  and *request_handler.cc* files located in the include and src directory respectively: 
	*  Create a new class that inherits from the RequestHandler base class. 
*  	Implement the `handle_request()` method in the derived class to provide the logic for handling the request and generating the response. 
	* Add the necessary constructor and member variables to the derived class to support any configuration or dependencies. 
* In the *request_handler_factory.cc* file, update the `create_handler()` function to add the 
* Document the new request handler in the code and provide relevant comments for better understanding. 

### Example Request Handler 
Here is an example of an existing request handler implementation:
 ``` cpp
 class ImageRequestHandler: public RequestHandler {
public:
    ImageRequestHandler(const std::string& path, const NginxConfig& config) {
        // Retrieve any necessary information from the config file
        // Example: Parse the config to get specific settings for the EchoHandler
    }

    status handle_request(const request<empty_body>& request, response<empty_body>& response) override {
        // Handle the request logic based on the parsed request
        // Generate the appropriate response

        // Example: Echo the request body in the response
        response.result(status::ok);
        response.body() = request.body();
        response.prepare_payload();

        return true; // Request handled successfully
    }
};
 ```
 In this example, the ImageRequestHandler is a request handler that echoes the request body in the response. It takes in a path and a config object as constructor parameters, allowing customization based on the configuration. The handle_request() method implements the logic to generate the appropriate response.

### Documentation 
For more detailed documentation, please refer to the inline comments in the source code files. They provide additional explanations, guidelines, and examples to help you understand and work with the codebase effectively.

License This project is licensed under the MIT License. You are free to use, modify, and distribute the code in accordance with the terms of the license.

Happy coding!