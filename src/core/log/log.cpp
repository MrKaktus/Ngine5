#include "core/log/context.h"

#ifdef EN_PLATFORM_ANDROID
#include <android/log.h>

#ifdef EN_DEBUG
#define  LOG_TAG    "Ngine 4 Debug" 
#else
#define  LOG_TAG    "Ngine 4" 
#endif
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#endif

namespace en
{
namespace log
{

#if defined(EN_PLATFORM_ANDROID) //|| defined(EN_PLATFORM_WINDOWS)
Context::Context() :
    initialized(false),
    loging(false),
    tofile(false),
    output(nullptr)
{
}

Context::~Context()
{
}

bool Context::create(void)
{
#if defined(EN_PLATFORM_ANDROID) 
    Log << "Starting module: Log.\n";
#endif

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
#ifdef EN_PLATFORM_ANDROID
    //std::cout.rdbuf(new en::log::androidbuf);
#endif

    return true;
}

void Context::init(void)
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

void Context::destroy(void)
{
//    if (initialized)
//    {
//        *output << "Closing module: Log.\n";
//        StorageContext.close(output);
//    }
#if defined(EN_PLATFORM_ANDROID)
//    delete std::cout.rdbuf(0);
#endif
}

#endif

#if defined(EN_PLATFORM_ANDROID)
void Interface::destination(uint8 dst)
{
    LogContext.tofile = false;
    if (dst == File)
    {
        LogContext.tofile = true;
   
        if (!LogContext.initialized)
        {
            LogContext.init();
        }
    }
}

uint8 Interface::destination(void)
{
    return LogContext.tofile ? File : Console;
}
   
void Interface::on(void)
{
    LogContext.loging = true;
   
    if (LogContext.tofile && !LogContext.initialized)
    {
        LogContext.init();
    }
}
   
void Interface::off(void)
{
    LogContext.loging = false;
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
#endif

#if defined(EN_PLATFORM_BLACKBERRY) || defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)

Context::Context() :
#if defined(EN_DEBUG)
    enabled(true),
#else
    enabled(false),
#endif
    destination(Console),
    output(nullptr),
    console(std::cout.rdbuf()),
    file(nullptr)
{
}

Context::~Context()
{
}

bool Context::create(void)
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
    return true;
}

bool Context::init(void)
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
            destination = File;         // Mark fact of logging to file
            std::cout << std::unitbuf;  // Ensure that log will be flushed immediatelly
            return true;
        }
    }

    return false;
}

void Context::destroy(void)
{
    std::cout << "Closing module: Log.\n";
    if (output)
    {
        std::cout.rdbuf(console);       // Restore cout's original streambuf
        output->close();   
        delete output;
    }
}

bool Interface::destination(Destination dst)
{
    // Switch loging from file to console
    if ( (dst == Console) &&
         (LogContext.destination == File) )
    {
        std::cout.rdbuf(LogContext.console);
        LogContext.destination = Console;
        return true;
    }

    // Switch loging from console to file
    if ( (dst == File) &&
         (LogContext.destination == Console) )
    {
        if (!LogContext.output)
        {
            return LogContext.init();
        }
        else
        {
            std::cout.rdbuf(LogContext.file);
            LogContext.destination = File;
            return true;
        }
    }

    return true;
}

Destination Interface::destination(void)
{
    return LogContext.destination;
}
    
void Interface::on(void)
{
    LogContext.enabled = true;
}

void Interface::off(void)
{
    LogContext.enabled = false;
}

Interface& Interface::operator << (const uint8& in)
{
    if (LogContext.enabled)
    {
        std::cout << static_cast<unsigned char>(in);
        std::cout.flush();
    }

    return *this;
}

Interface& Interface::operator << (const uint16& in)
{
    if (LogContext.enabled)
    {
        std::cout << static_cast<unsigned short>(in);
        std::cout.flush();
    }

    return *this;
}

Interface& Interface::operator << (const uint32& in)
{
    if (LogContext.enabled)
    {
        std::cout << static_cast<unsigned int>(in);
        std::cout.flush();
    }

    return *this;
}

Interface& Interface::operator << (const uint64& in)
{
    if (LogContext.enabled)
    {
        std::cout << static_cast<unsigned long long int>(in);
        std::cout.flush();
    }

    return *this;
}

Interface& Interface::operator << (const sint8& in)
{
    if (LogContext.enabled)
    {
        std::cout << static_cast<char>(in);
        std::cout.flush();
    }

    return *this;
}

Interface& Interface::operator << (const sint16& in)
{
    if (LogContext.enabled)
    {
        std::cout << static_cast<short>(in);
        std::cout.flush();
    }

    return *this;
}

Interface& Interface::operator << (const sint32& in)
{
    if (LogContext.enabled)
    {
        std::cout << static_cast<int>(in);
        std::cout.flush();
    }

    return *this;
}

Interface& Interface::operator << (const sint64& in)
{
    if (LogContext.enabled)
    {
        std::cout << static_cast<long long int>(in);
        std::cout.flush();
    }

    return *this;
}

Interface& Interface::operator << (const float& in)
{
    if (LogContext.enabled)
    {
        std::cout << in;
        std::cout.flush();
    }

    return *this;
}

Interface& Interface::operator << (const double& in)
{
    if (LogContext.enabled)
    {
        std::cout << in;
        std::cout.flush();
    }

    return *this;
}

Interface& Interface::operator << (const char in)
{
    if (LogContext.enabled)
    {
        std::cout << in;
        std::cout.flush();
    }

    return *this;
}

Interface& Interface::operator << (const char* in)
{
    if (LogContext.enabled)
    {
        std::cout << in;
        std::cout.flush();
    }

    return *this;
}
 
Interface& Interface::operator << (const std::string in)
{
    if (LogContext.enabled)
    {
        std::cout << in.c_str();
        std::cout.flush();
    }

    return *this;
}

Interface& Interface::operator << (StreamFuncPtr function)
{
    if (LogContext.enabled)
    {
        function(std::cout);
    }

    return *this;
}
#endif

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
//#if defined(EN_PLATFORM_BLACKBERRY) || defined(EN_PLATFORM_WINDOWS)
//    if (m_tofile && m_initialized)
//       function(*m_output);
//    else
//       function(std::cout);
//#endif
//    }
// return *this;
//}
//#endif

} // en::log

log::Context   LogContext;
log::Interface Log;

} // en
