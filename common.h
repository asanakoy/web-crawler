#ifndef TMK_COMMON_H
#define TMK_COMMON_H

#include <stdio.h>
#include <stdarg.h>
#include <string>

#define TMK_LOG(...) Logger::writeToLog(__VA_ARGS__)
#define TMK_FLOG(...) Logger::writeToFileLog(__VA_ARGS__)
#define TMK_LOG_ALL(...) do {Logger::writeToLog(__VA_ARGS__); Logger::writeToFileLog(__VA_ARGS__);} while(0)

class Logger {
public:

    static void setOutputFile(const std::string& path) {
        pLogFile = fopen(path.c_str(),"w");
    }

    static inline void writeToLog(const char *format, ...) {
        va_list     args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    } 
    
    static inline void writeToFileLog(const char *format, ...) {
        va_list     args;
        va_start(args, format);
        vfprintf(Logger::pLogFile, format, args);
        va_end(args);
    } 

private:
        static FILE* pLogFile;
};

#endif // TMK_COMMON_H