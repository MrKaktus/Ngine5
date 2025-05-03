/*

 Ngine v5.0

 Module      : Log
 Requirements: storage
 Description : Stores in log file, all communicates
               from engine routines and user app.
               It can be used to trace errors or
               execution flow.

*/

#ifndef ENG_CORE_LOG_PRINT
#define ENG_CORE_LOG_PRINT

#include "core/log/log.h"

namespace en
{
namespace log
{

// Debug   - by default logs to Console
// Release - by default disabled
class PrintLog : public Interface
{
private:

    bool        enabled;     // Is logging enabled
    Destination mode;        // Logging destination, by default
    FILE*       file;

public:

    PrintLog();
    ~PrintLog();

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

    // Print style logging

    virtual void operator()(const char* format, ...);
    virtual void error(const char* format, ...);
};

} // en::log
} // en

#endif