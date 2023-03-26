#ifndef ERRORLOGGER_H
#define ERRORLOGGER_H


#include <string>
#include <stdexcept>


namespace ErrorLogger
{
    class debugException : public std::runtime_error
    {
    private:
        std::string msg;
    public:
        debugException(const std::string &arg, const char *file, int line) : std::runtime_error(arg);
        ~debugException();

        const char *what() const throw();
    };
};
#define throwDebugException(arg) throw ErrorLogger::debugException(arg, __FILE__, __LINE__);


#endif ERRORLOGGER_H
