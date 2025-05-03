/*

 Ngine v5.0

 Module      : Log
 Requirements: storage
 Description : Stores in log file, all communicates
               from engine routines and user app.
               It can be used to trace errors or
               execution flow.

*/

#include "core/log/andLog.h"

#if defined(EN_PLATFORM_ANDROID)

#include <android/log.h>

#ifdef EN_DEBUG
#define  LOG_TAG    "Ngine 4 Debug"
#else
#define  LOG_TAG    "Ngine 4"
#endif

#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

AndLog::AndLog() :
    initialized(false),
    loging(false),
    tofile(false),
    output(nullptr)
{
}

AndLog::~AndLog()
{
}

bool AndLog::create(void)
{
    enLog << "Starting module: Log.\n";

    std::string destination;
    if (Config.get("en.core.log.destination", destination))
    {
        loging = true;
        if (destination == "File")
        {
            tofile = true;
            init();
        }
    }

    //std::cout.rdbuf(new en::log::androidbuf);

    return true;
}

void AndLog::init(void)
{
//    string filename;
//    if (Config.get("en.core.log.file", filename))
//    {
//        filename = "./" + filename;
//        output = StorageContext.openForOverwriting(filename);
//        if (output)
//        {
        initialized = true;
//            *output << "Starting module: Log.\n";
//            *output << unitbuf;
//        }
//    }
}

void AndLog::destroy(void)
{
//    if (initialized)
//    {
//        *output << "Closing module: Log.\n";
//        StorageContext.close(output);
//    }

//    delete std::cout.rdbuf(0);
}

namespace en
{
namespace log
{

void Interface::destination(uint8 dst)
{
    tofile = false;
    if (dst == File)
    {
        tofile = true;

        if (!initialized)
        {
            init();
        }
    }
}

uint8 Interface::destination(void)
{
    return tofile ? File : Console;
}

void Interface::on(void)
{
    loging = true;

    if (tofile && !initialized)
    {
        init();
    }
}

void Interface::off(void)
{
    loging = false;
}

Interface& Interface::operator << (const uint8& in)
{
    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "%u", in);
    return *this;
}

Interface& Interface::operator << (const uint16& in)
{
    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "%u", in);
    return *this;
}

Interface& Interface::operator << (const uint32& in)
{
    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "%u", in);
    return *this;
}

Interface& Interface::operator << (const uint64& in)
{
    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "%u", in);
    return *this;
}

Interface& Interface::operator << (const sint8& in)
{
    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "%i", in);
    return *this;
}

Interface& Interface::operator << (const sint16& in)
{
    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "%i", in);
    return *this;
}

Interface& Interface::operator << (const sint32& in)
{
    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "%i", in);
    return *this;
}

Interface& Interface::operator << (const sint64& in)
{
    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "%i", in);
    return *this;
}

Interface& Interface::operator << (const char* in)
{
    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "%s", in);
    return *this;
}

Interface& Interface::operator << (const std::string in)
{
    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "%s", in.c_str());
    return *this;
}

Interface& Interface::operator << (StreamFuncPtr function)
{
    return *this;
}

} // en::log
} // en

#endif