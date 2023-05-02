#ifndef LOGGING_H
#define LOGGING_H

#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/settings.hpp>
#include <boost/log/trivial.hpp>

class Logging
{
public:
    static void init_log();
    static void termintion_log(int);
};

#endif // LOGGING_H