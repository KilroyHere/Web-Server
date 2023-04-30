cmake_minimum_required(VERSION 3.10.0)
project(git-gud)
# Make sure developers do not run cmake in the main project directory, to keep
# build artifacts from becoming clutter
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source builds not allowed.
    Please make a new directory (called a build directory) and run CMake from there.
    You may need to remove CMakeCache.txt." )
endif()
# Turn on debug builds if we are building in a devel shell
if (CMAKE_BUILD_TYPE STREQUAL "" AND "$ENV{DEVEL_SHELL}" STREQUAL "1")
    message(STATUS "Setting debug build type by default in devel shell")
    set(CMAKE_BUILD_TYPE Debug)
endif()
# Output binaries to a sub directory "bin"
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# pthread linking
set(CMAKE_CXX_FLAGS "-pthread")
# Enable cmake testing
include(CTest)
enable_testing()
# Enable GoogleTest
include(GoogleTest)
add_subdirectory(/usr/src/googletest googletest)
# Enable Boost
# Use static libraries so binaries can be deployed without a full boost install
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.50 REQUIRED COMPONENTS system)
message(STATUS "Boost version: ${Boost_VERSION}")

include_directories(include)
add_library(http_server_lib src/http_server.cc)
add_library(session_lib src/session.cc)
add_library(request_parser_lib src/request_parser.cc)
add_library(http_request_lib src/http_request.cc)
add_library(http_response_lib src/http_response.cc)
add_library(request_handler_lib  src/request_handler.cc)
add_library(response_handler_lib  src/response_handler.cc)
add_library(config_parser_lib  config/config_parser.cc)

add_executable(server src/server_main.cc)
target_link_libraries(server Boost::system http_server_lib session_lib  request_handler_lib response_handler_lib request_parser_lib http_response_lib http_request_lib  config_parser_lib )

# TODO: Commented out the tests as they need to be updates to match the updated constructors.
# add_executable(config_parser_test tests/config_parser_test.cc)
# target_link_libraries(config_parser_test config_parser_lib gtest_main)

# add_executable(request_handler_test tests/request_handler_test.cc)
# target_link_libraries(request_handler_test  request_handler_lib request_parser_lib response_handler_lib http_request_lib http_response_lib gtest_main)

# add_executable(session_test tests/session_test.cc)
# target_link_libraries(session_test http_server_lib session_lib  request_handler_lib  response_handler_lib request_parser_lib http_request_lib http_response_lib gtest_main)

# gtest_discover_tests(config_parser_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
# gtest_discover_tests(request_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
# gtest_discover_tests(session_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)


#Generate Unit Test Coverage Report 
 include(cmake/CodeCoverageReportConfig.cmake)
 generate_coverage_report(
    # TARGETS config_parser_lib session_lib http_request_lib http_response_lib http_server_lib request_handler_lib request_parser_lib server
    # TESTS config_parser_test request_handler_test session_test
)

# if(NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Coverage")
#     add_test(NAME web_test
#         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/test.sh
#         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests
#     )
# endif()
 
