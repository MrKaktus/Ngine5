/*

 Ngine v5.0

 Module      : Log
 Requirements: storage
 Description : Stores in log file, all communicates
               from engine routines and user app.
               It can be used to trace errors or
               execution flow.

*/

#ifndef ENG_CORE_LOG_STREAM
#define ENG_CORE_LOG_STREAM

#include "core/log/log.h"

#if defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)

#include <iostream>
#include <fstream>

#include <iomanip>
//#include <streambuf>

namespace en
{
namespace log
{

class StreamLog : public Interface
{
private:

    bool        enabled;     // Is logging enabled
    Destination mode;        // Logging destination, by default
    // Debug   - logs to Console
    // Release - logs to File
    std::ofstream*  output;  // Output file
    std::streambuf* console; // Console output stream
    std::streambuf* file;    // File output stream

public:

    StreamLog();
    ~StreamLog();

    void ComLog(void);

    bool init(void);
    void destroy(void);

    virtual bool        destination(Destination dst);      // Sets loging destination (can fail if log file cannot be opened)
    virtual Destination destination(void);                 // Gets loging destination
    virtual void        on(void);                          // Turns loging on
    virtual void        off(void);                         // Turns loging off 

    // Stream style logging

    virtual Interface& operator << (const uint8& in);
    virtual Interface& operator << (const uint16& in);
    virtual Interface& operator << (const uint32& in);
    virtual Interface& operator << (const uint64& in);
    virtual Interface& operator << (const sint8& in);
    virtual Interface& operator << (const sint16& in);
    virtual Interface& operator << (const sint32& in);
    virtual Interface& operator << (const sint64& in);
    virtual Interface& operator << (const float& in);
    virtual Interface& operator << (const double& in);
    virtual Interface& operator << (const char in);
    virtual Interface& operator << (const char* in);
    virtual Interface& operator << (const std::string in);

    virtual Interface& operator << (StreamFuncPtr function);  // Accepts stream functions to log

    // Print style logging (does not support logging to file)

    virtual void operator()(const char* format, ...);
    virtual void error(const char* format, ...);
};

} // en::log
} // en

#endif
#endif