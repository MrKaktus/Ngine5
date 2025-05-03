/*

 Ngine v5.0

 Module      : Log
 Requirements: storage
 Description : Stores in log file, all communicates
               from engine routines and user app.
               It can be used to trace errors or
               execution flow.

*/

#ifndef ENG_CORE_LOG_ANDROID
#define ENG_CORE_LOG_ANDROID

#include "core/log/log.h"

#if defined(EN_PLATFORM_ANDROID)

namespace en
{
namespace log
{

class AndLog : public Interface
{
private:

    bool        enabled;     // Is logging enabled
    Destination destination; // Logging destination, by default
    // Debug   - logs to Console
    // Release - logs to File
    std::ofstream* output;  // Output file
    std::streambuf* console; // Console output stream
    std::streambuf* file;    // File output stream

    void printTo(FILE* stream, const char* format, ...);
    void printError(const char* format, ...);

public:

    AndLog();
    ~AndLog();

    bool create(void);
    bool init(void);
    void destroy(void);
};

} // en::log
} // en

#endif
#endif