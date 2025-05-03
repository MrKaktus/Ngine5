/*

 Ngine v5.0

 Module      : Log
 Requirements: storage
 Description : Stores in log file, all communicates
               from engine routines and user app.
               It can be used to trace errors or
               execution flow.

*/

#include "core/log/log.h"
#include "core/log/StreamLog.h"
#include "core/log/PrintLog.h"

namespace en
{
namespace log
{

bool Interface::create(void)
{
    if (Log)
    {
        return true;
    }

#if defined(EN_PLATFORM_ANDROID)
    Log = std::make_unique<AndLog>();
#endif
#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)
    Log = std::make_unique<StreamLog>();
#endif
#if defined(EN_PLATFORM_WINDOWS)
    //Log = std::make_unique<StreamLog>();
    Log = std::make_unique<PrintLog>();
#endif

    return (Log == nullptr) ? false : true;
}

} // en::log

std::unique_ptr<log::Interface> Log;

} // en
