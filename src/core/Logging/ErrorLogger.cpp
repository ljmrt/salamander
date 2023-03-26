#include <core/Logging/ErrorLogger.h>

#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>


ErrorLogger::debugException::debugException(const std::string &arg, const char *file, int line) : std::runtime_error(arg)
{
    std::ostringstream << "Logging::"
                       << file
                       << "::"
                       << line
                       << "::\""
                       << arg
                       << "\"";
}

ErrorLogger::debugException::~debugException()
{
}

ErrorLogger::debugException::const char *what() const throw()
{
    return msg.c_str();
}
