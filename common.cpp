#include "common.h"

FILE* Logger::pLogFile = fopen("log.txt", "w");

namespace tmk {
    
    void logVector(const std::vector<size_t>& data, bool log_all) {
        if (log_all) {
            TMK_LOG_ALL("[ ");
            for (auto it = data.begin(); it != data.end(); ++it) {
                TMK_LOG_ALL("%zu, ", *it);
            }
            TMK_LOG_ALL(" ]\n");
        } else {
            TMK_LOG("[ ");
            for (auto it = data.begin(); it != data.end(); ++it) {
                TMK_LOG("%zu, ", *it);
            }
            TMK_LOG(" ]\n");
        }
    }
} // tmk
