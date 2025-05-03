/*

 Ngine v5.0

 Module      : Log
 Requirements: storage
 Description : Stores in log file, all communicates
               from engine routines and user app.
               It can be used to trace errors or
               execution flow.

*/

#include "core/log/StreamLog.h"

#if defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)

#include <assert.h>

namespace en
{
namespace log
{

StreamLog::StreamLog() :
#if defined(EN_DEBUG)
    enabled(true),
#else
    enabled(false),
#endif
    mode(Console),
    output(nullptr),
    console(std::cout.rdbuf()),
    file(nullptr)
{
    ComLog();
}

// TODO: This should be ComLog base class constructor
void StreamLog::ComLog(void)
{
    std::string dst;
    if (Config.get("en.core.log.destination", dst))
    {
        if (dst == "File")
        {
            if (init())
            {
                enabled = true;          // Override logging in Release mode 
            }
        }
    }

    std::cout << "Starting module: Log.\n";
}

StreamLog::~StreamLog()
{
}

bool StreamLog::init(void)
{
    std::string filename;
    if (!Config.get("en.core.log.file", filename))  // WA: Until Config is fixed!
    {
        filename = "./log.txt";
    }

    {
        // This function should be called only when file is not initialized yet
        output = new std::ofstream(filename.c_str());
        if (output)
        {
            file = output->rdbuf();     // Get file's streambuf
            std::cout.rdbuf(file);      // Assign cout to log file
            mode = File;                // Mark fact of logging to file
            std::cout << std::unitbuf;  // Ensure that log will be flushed immediatelly
            return true;
        }
    }

    return false;
}

void StreamLog::destroy(void)
{
    std::cout << "Closing module: Log.\n";
    if (output)
    {
        std::cout.rdbuf(console);       // Restore cout's original streambuf
        output->close();
        delete output;
    }
}

bool StreamLog::destination(Destination dst)
{
    // Switch loging from file to console
    if ((dst == Console) &&
        (mode == File))
    {
        std::cout.rdbuf(console);
        mode = Console;
        return true;
    }

    // Switch loging from console to file
    if ((dst == File) &&
        (mode == Console))
    {
        if (!output)
        {
            return init();
        }
        else
        {
            std::cout.rdbuf(file);
            mode = File;
            return true;
        }
    }

    return true;
}

Destination StreamLog::destination(void)
{
    return mode;
}

void StreamLog::on(void)
{
    enabled = true;
}

void StreamLog::off(void)
{
    enabled = false;
}

Interface& StreamLog::operator << (const uint8& in)
{
    if (enabled)
    {
        std::cout << static_cast<unsigned char>(in);
        std::cout.flush();
    }

    return *this;
}

Interface& StreamLog::operator << (const uint16& in)
{
    if (enabled)
    {
        std::cout << static_cast<unsigned short>(in);
        std::cout.flush();
    }

    return *this;
}

Interface& StreamLog::operator << (const uint32& in)
{
    if (enabled)
    {
        std::cout << static_cast<unsigned int>(in);
        std::cout.flush();
    }

    return *this;
}

Interface& StreamLog::operator << (const uint64& in)
{
    if (enabled)
    {
        std::cout << static_cast<unsigned long long int>(in);
        std::cout.flush();
    }

    return *this;
}

Interface& StreamLog::operator << (const sint8& in)
{
    if (enabled)
    {
        std::cout << static_cast<char>(in);
        std::cout.flush();
    }

    return *this;
}

Interface& StreamLog::operator << (const sint16& in)
{
    if (enabled)
    {
        std::cout << static_cast<short>(in);
        std::cout.flush();
    }

    return *this;
}

Interface& StreamLog::operator << (const sint32& in)
{
    if (enabled)
    {
        std::cout << static_cast<int>(in);
        std::cout.flush();
    }

    return *this;
}

Interface& StreamLog::operator << (const sint64& in)
{
    if (enabled)
    {
        std::cout << static_cast<long long int>(in);
        std::cout.flush();
    }

    return *this;
}

Interface& StreamLog::operator << (const float& in)
{
    if (enabled)
    {
        std::cout << in;
        std::cout.flush();
    }

    return *this;
}

Interface& StreamLog::operator << (const double& in)
{
    if (enabled)
    {
        std::cout << in;
        std::cout.flush();
    }

    return *this;
}

Interface& StreamLog::operator << (const char in)
{
    if (enabled)
    {
        std::cout << in;
        std::cout.flush();
    }

    return *this;
}

Interface& StreamLog::operator << (const char* in)
{
    if (enabled)
    {
        std::cout << in;
        std::cout.flush();
    }

    return *this;
}

Interface& StreamLog::operator << (const std::string in)
{
    if (enabled)
    {
        std::cout << in.c_str();
        std::cout.flush();
    }

    return *this;
}

Interface& StreamLog::operator << (StreamFuncPtr function)
{
    if (enabled)
    {
        function(std::cout);
    }

    return *this;
}

#include <stdio.h>
#include <stdarg.h>

void StreamLog::operator()(const char* format, ...)
{
    if (enabled)
    {
        va_list args;
        va_start(args, format);

        if (mode == File)
        {
            // Not supported!
            assert(0);
        }
        else
        {
            assert(mode == Console);

            vfprintf(stdout, format, args);
        }

        va_end(args);
    }
}

void StreamLog::error(const char* format, ...)
{
    if (enabled)
    {
        va_list args;
        va_start(args, format);

        if (mode == File)
        {
            // Not supported!
            assert(0);

            //fprintf(file, "[ERROR]");
            //vfprintf(file, format, args);
            //fflush(file);
        }
        else
        {
            assert(mode == Console);

            fprintf(stderr, "[ERROR]");
            vfprintf(stderr, format, args);
            fflush(stderr);
        }

        va_end(args);
    }
}

//#if defined(EN_PLATFORM_WINDOWS)
//NLogContext::NLogContext()
//{
// uint8 destination;
// Config.get("en.core.log.destination", &destination);
//
// m_loging      = false;
// m_tofile      = false;
// m_initialized = false;
// m_output      = NULL;
//
// if (destination)
//    m_loging = true;
// if (destination == File)
//    m_tofile = true;
//
// if (m_loging && m_tofile)
//    init();
//}
//
//NLogContext::~NLogContext() 
//{
// if (m_initialized)
//    StorageContext.close(m_output);
//}
//
//NLogContext& NLogContext::getInstance(void) 
//{
// static NLogContext instance;
// return instance;
//}
//
//NLogContext& NLogContext::operator << (StreamFuncPtr function)
//{
// if (m_loging)
//    {
//#if defined(EN_PLATFORM_WINDOWS)
//    if (m_tofile && m_initialized)
//       function(*m_output);
//    else
//       function(std::cout);
//#endif
//    }
// return *this;
//}
//#endif
//*/
//*/


} // en::log
} // en

#endif