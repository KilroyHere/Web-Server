// Adapted from
// https://www.boost.org/doc/libs/1_82_0/libs/log/example/doc/tutorial_file.cpp
/*
 *          Copyright Andrey Semashev 2007 - 2015.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */

#include "logging.h"

void Logging::init_log()
{
    static const std::string ENTRY_FORMAT("[%TimeStamp%][%ThreadID%][%Severity%]: %Message%");
    boost::log::add_file_log(
        boost::log::keywords::file_name = "../logs/sample_%N.log",
        boost::log::keywords::rotation_size = 10 * 1024 * 1024,
        boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
        boost::log::keywords::format = ENTRY_FORMAT,
        boost::log::keywords::auto_flush = true);
    boost::log::add_console_log(std::cerr,
                                boost::log::keywords::format = ENTRY_FORMAT);
    boost::log::core::get()->set_filter(
        boost::log::trivial::severity >= boost::log::trivial::info);
    boost::log::register_simple_formatter_factory<boost::log::trivial::severity_level, char>("Severity");
    boost::log::add_common_attributes();
};

void Logging::termintion_log(int i)
{
    BOOST_LOG_TRIVIAL(fatal) << "Server terminated by signal handler.";
    exit(i);
}