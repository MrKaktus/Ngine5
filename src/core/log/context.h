/*

 Ngine v5.0
 
 Module      : Log 
 Requirements: storage
 Description : Stores in log file, all communicates
               from engine routines and user app. 
               It can be used to trace errors or
               execution flow.

*/

#ifndef ENG_CORE_LOG_CONTEXT
#define ENG_CORE_LOG_CONTEXT

#include "core/defines.h"
#include "core/types.h"

#include "core/log/log.h"

#include <iostream>
#include <fstream>

#include <iomanip>
//#include <streambuf>
using namespace std;

namespace en
{
   namespace log
   {
#ifdef EN_PLATFORM_ANDROID
//class androidbuf : public std::streambuf 
//      {
//      public:
//      typedef std::streambuf _Base;
//      
//      typedef _Base::int_type int_type;
//      typedef _Base::traits_type traits_type;
//
//      enum { bufsize = 4096 }; // ... or some other suitable buffer size
//
//      androidbuf() 
//      { 
//      this->setp(buffer_, buffer + bufsize - 1); 
//      }
//
//      private:
//
//      int overflow(int c) 
//      {
//      if (c == traits_type::eof()) 
//         { 
//         *this->pptr() = traits_type::to_char_type(c);
//         this->sbumpc();
//         }
//      return this->sync()? traits_type::eof(): traits_type::not_eof(c);
//      }
//
//      int sync() 
//      {
//      int rc = 0;
//      if (this->pbase() != this->pptr()) 
//         {
//         this->write_to_android_log(this->pbase(), this->pptr() - this->pbase());
//         rc = write_successul;
//         this->setp(buffer, buffer + bufsize - 1);
//         }
//      return rc;
//      }
//
//      char buffer[bufsize];
//      };
#endif

#if defined(EN_PLATFORM_ANDROID)// || defined(EN_PLATFORM_WINDOWS)
   struct Context
          {
          bool     initialized; // true  - Log file is open
          bool     loging;      // Turns logging on or off
          bool     tofile;      // true  - Logging to file
                                // false - Logging to console
          fstream* output; // Output file stream

          Context();
         ~Context();

          bool create(void);
          void init(void);
          void destroy(void);
          };
#endif

#if defined(EN_PLATFORM_BLACKBERRY) || defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)
   struct Context
          {
          bool        enabled;     // Is logging enabled
          Destination destination; // Logging destination, by default
                                   // Debug   - logs to Console
                                   // Release - logs to File
          ofstream*   output;      // Output file
          streambuf*  console;     // Console output stream
          streambuf*  file;        // File output stream

          Context();
         ~Context();

          bool create(void);
          bool init(void);
          void destroy(void);
          };
#endif
   }

extern log::Context LogContext;
}

#endif