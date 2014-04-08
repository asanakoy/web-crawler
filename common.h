#ifndef TMK_COMMON_H
#define TMK_COMMON_H

#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <vector>
#include <fstream>

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

namespace tmk {
    template <typename T>
    void saveToDisk(const std::vector<T>& data, const char* filePath) {
        std::ofstream fout(filePath);
        if (!fout) {
            TMK_LOG_ALL("Error: failed to open file %s\n", filePath);
        }
        TMK_LOG_ALL("Writing to file %s\n", filePath);
        for (auto it = data.begin(); it != data.end(); ++it) 
        {
             fout << *it << std::endl;
        }
    }
    
    void logVector(const std::vector<size_t>& data, bool log_all=false);
}; // tmk

#endif // TMK_COMMON_H
