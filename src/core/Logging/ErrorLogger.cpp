#include <core/Logging/ErrorLogger.h>

#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>


ErrorLogger::debugException::debugException(const std::string &arg, const char *file, int line) : std::runtime_error(arg)
{
    std::cout << "\033[35m" << std::endl;  // set output foreground color to magenta.
        
    std::ostringstream o;
    o << "Logging::"
      << file
      << "::"
      << line
      << "::\""
      << arg
      << "\"";
}

ErrorLogger::debugException::~debugException() throw()
{
}

const char *ErrorLogger::debugException::what() const throw()
{
    return msg.c_str();
}
