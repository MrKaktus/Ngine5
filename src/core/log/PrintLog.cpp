/*

 Ngine v5.0

 Module      : Log
 Requirements: storage
 Description : Stores in log file, all communicates
               from engine routines and user app.
               It can be used to trace errors or
               execution flow.

*/

#include "core/log/PrintLog.h"

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

namespace en
{
namespace log
{

PrintLog::PrintLog() :
#if defined(EN_DEBUG)
    enabled(true),
#else
    enabled(false),
#endif
    mode(Console),
    file(nullptr)
{
    ComLog();
}

// TODO: This should be ComLog base class constructor
void PrintLog::ComLog(void)
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

PrintLog::~PrintLog()
{
    destroy();
}

bool PrintLog::init(void)
{
    std::string filename;
    if (!Config.get("en.core.log.file", filename))  // WA: Until Config is fixed!
    {
        filename = "./log.txt";
    }

    // We could override stdout to point to selected file,
    // which would result in printf() writing directly to
    // selected engine log output, but that would mean
    // implicit behavior that may not be desired by developer.
    errno_t result = fopen_s(&file, filename.c_str(), "w");
    if (result != 0 || !file)
    {
        // EINVAL
        return false;
    }

    mode = File;
    return true;
}

void PrintLog::destroy(void)
{
    std::cout << "Closing module: Log.\n";
    if (file)
    {
        fflush(file);
        fclose(file);
    }
}

bool PrintLog::destination(Destination dst)
{
    // Switch loging from file to console
    if ((dst == Console) &&
        (mode == File))
    {
        mode = Console;
        return true;
    }

    // Switch loging from console to file
    if ((dst == File) &&
        (mode == Console))
    {
        if (!file)
        {
            return init();
        }
        else
        {
            mode = File;
            return true;
        }
    }

    return true;
}

Destination PrintLog::destination(void)
{
    return mode;
}

void PrintLog::on(void)
{
    enabled = true;
}

void PrintLog::off(void)
{
    enabled = false;
}

Interface& PrintLog::operator << (const uint8& in)
{
    if (enabled)
    {
        if (mode == File && file)
        {
            fprintf(file, "%c", in);
            fflush(file);
        }
        else
        {
            assert(mode == Console);

            printf("%c", in);
            fflush(stdout);
        }
    }

    return *this;
}

Interface& PrintLog::operator << (const uint16& in)
{
    if (enabled)
    {
        if (mode == File && file)
        {
            fprintf(file, "%u", in);
            fflush(file);
        }
        else
        {
            assert(mode == Console);

            printf("%u", in);
            fflush(stdout);
        }
    }

    return *this;
}

Interface& PrintLog::operator << (const uint32& in)
{
    if (enabled)
    {
        if (mode == File && file)
        {
            fprintf(file, "%u", in);
            fflush(file);
        }
        else
        {
            assert(mode == Console);

            printf("%u", in);
            fflush(stdout);
        }
    }

    return *this;
}

Interface& PrintLog::operator << (const uint64& in)
{
    if (enabled)
    {
        if (mode == File && file)
        {
            fprintf(file, "%llu", in);
            fflush(file);
        }
        else
        {
            assert(mode == Console);

            printf("%llu", in);
            fflush(stdout);
        }
    }

    return *this;
}

Interface& PrintLog::operator << (const sint8& in)
{
    if (enabled)
    {
        if (mode == File && file)
        {
            fprintf(file, "%c", in);
            fflush(file);
        }
        else
        {
            assert(mode == Console);

            printf("%c", in);
            fflush(stdout);
        }
    }

    return *this;
}

Interface& PrintLog::operator << (const sint16& in)
{
    if (enabled)
    {
        if (mode == File && file)
        {
            fprintf(file, "%i", in);
            fflush(file);
        }
        else
        {
            assert(mode == Console);

            printf("%i", in);
            fflush(stdout);
        }
    }

    return *this;
}

Interface& PrintLog::operator << (const sint32& in)
{
    if (enabled)
    {
        if (mode == File && file)
        {
            fprintf(file, "%i", in);
            fflush(file);
        }
        else
        {
            assert(mode == Console);

            printf("%i", in);
            fflush(stdout);
        }
    }

    return *this;
}

Interface& PrintLog::operator << (const sint64& in)
{
    if (enabled)
    {
        if (mode == File && file)
        {
            fprintf(file, "%lli", in);
            fflush(file);
        }
        else
        {
            assert(mode == Console);

            printf("%lli", in);
            fflush(stdout);
        }
    }

    return *this;
}

Interface& PrintLog::operator << (const float& in)
{
    if (enabled)
    {
        if (mode == File && file)
        {
            fprintf(file, "%f", in);
            fflush(file);
        }
        else
        {
            assert(mode == Console);

            printf("%f", in);
            fflush(stdout);
        }
    }

    return *this;
}

Interface& PrintLog::operator << (const double& in)
{
    if (enabled)
    {
        if (mode == File && file)
        {
            fprintf(file, "%lf", in);
            fflush(file);
        }
        else
        {
            assert(mode == Console);

            printf("%lf", in);
            fflush(stdout);
        }
    }

    return *this;
}

Interface& PrintLog::operator << (const char in)
{
    if (enabled)
    {
        if (mode == File && file)
        {
            fprintf(file, "%c", in);
            fflush(file);
        }
        else
        {
            assert(mode == Console);

            printf("%c", in);
            fflush(stdout);
        }
    }

    return *this;
}

Interface& PrintLog::operator << (const char* in)
{
    if (enabled)
    {
        if (mode == File && file)
        {
            fprintf(file, "%s", in);
            fflush(file);
        }
        else
        {
            assert(mode == Console);

            printf("%s", in);
            fflush(stdout);
        }
    }

    return *this;
}

Interface& PrintLog::operator << (const std::string in)
{
    if (enabled)
    {
        if (mode == File && file)
        {
            fprintf(file, "%s", in.c_str());
            fflush(file);
        }
        else
        {
            assert(mode == Console);

            printf("%s", in.c_str());
            fflush(stdout);
        }
    }

    return *this;
}

Interface& PrintLog::operator << (StreamFuncPtr function)
{
    if (enabled)
    {
        if (function == std::endl)
        {
            if (mode == File && file)
            {
                fprintf(file, "\n");
                fflush(file);
            }
            else
            {
                assert(mode == Console);

                printf("\n");
                fflush(stdout);
            }
        }
        else
        {
            // Not supported!
            assert( 0 );
        }
    }

    return *this;
}

void PrintLog::operator()(const char* format, ...)
{
    if (enabled)
    {
        va_list args;
        va_start(args, format);

        if (mode == File && file)
        {
            vfprintf(file, format, args);
            fflush(file);
        }
        else
        {
            assert(mode == Console);

            vfprintf(stdout, format, args);
            fflush(stdout);
        }

        va_end(args);
    }
}

void PrintLog::error(const char* format, ...)
{
    if (enabled)
    {
        va_list args;
        va_start(args, format);

        if (mode == File && file)
        {
            fprintf(file, "[ERROR]");
            vfprintf(file, format, args);
            fflush(file);
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

} // en::log
} // en
