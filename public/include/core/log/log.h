/*

 Ngine v5.0
 
 Module      : Log 
 Requirements: storage
 Description : Stores in log file, all communicates
               from engine routines and user app. 
               It can be used to trace errors or
               execution flow.

*/

#ifndef ENG_CORE_LOG
#define ENG_CORE_LOG

#include <iostream>
#include <iomanip>

#include "core/defines.h"
#include "core/types.h"

//#include "core/storage/context.h"
#include "core/config/config.h"

namespace en
{
   namespace log
   {
   enum Destination
      { 
      Console = 0,
      File     
      };

#ifdef EN_PLATFORM_ANDROID

   struct Interface
      {
      // Declaration of standard stream output function pointers
      typedef std::basic_ostream<char, std::char_traits<char> > StreamOutType;
      typedef StreamOutType& (*StreamFuncPtr)(StreamOutType&);
      
      void  destination(uint8 dst);           // Sets loging destination
      uint8 destination(void);                // Gets loging destination
      void  on(void);                         // Turns loging on
      void  off(void);                        // Turns loging off
      
      Interface& operator << (const uint8& in);
      Interface& operator << (const uint16& in);
      Interface& operator << (const uint32& in);
      Interface& operator << (const uint64& in);
      Interface& operator << (const sint8& in);
      Interface& operator << (const sint16& in);
      Interface& operator << (const sint32& in);
      Interface& operator << (const sint64& in);
      //Interface& operator << (const float& in);
      //Interface& operator << (const double& in);
      Interface& operator << (const char* in);
      Interface& operator << (const std::string in);
      
      template <typename T>
      Interface& operator << (const T& content);        
      Interface& operator << (StreamFuncPtr function);
      };

   template <typename T>
   Interface& Interface::operator << (const T& content)
   {
   return *this;
   }

#endif

#if defined(EN_PLATFORM_BLACKBERRY) || defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)

   // In Debug mode logging is enabled by default to console window.
   // In Release mode logging is disabled by default. It can be enabled from config file, to log to log file.
   class Interface 
      {
      private:
      // Declaration of standard stream output function pointers
      typedef std::basic_ostream<char, std::char_traits<char> > StreamOutType;
      typedef StreamOutType& (*StreamFuncPtr)(StreamOutType&);
      
      public:
      bool        destination(Destination dst);      // Sets loging destination (can fail if log file cannot be opened)
      Destination destination(void);                 // Gets loging destination
      void        on(void);                             // Turns loging on
      void        off(void);                            // Turns loging off 
      
      Interface& operator << (const uint8& in);
      Interface& operator << (const uint16& in);
      Interface& operator << (const uint32& in);
      Interface& operator << (const uint64& in);
      Interface& operator << (const sint8& in);
      Interface& operator << (const sint16& in);
      Interface& operator << (const sint32& in);
      Interface& operator << (const sint64& in);
      Interface& operator << (const float& in);
      Interface& operator << (const double& in);
      Interface& operator << (const char in);
      Interface& operator << (const char* in);
      Interface& operator << (const std::string in);
      
      template <typename T>
      Interface& operator << (const T& content);        // Accepts streams to log
      Interface& operator << (StreamFuncPtr function);  // Accepts stream functions to log                                      
      };
      
   template <typename T>
   Interface& Interface::operator << (const T& content)
   {
   std::cout << content;
   return *this;
   }

#endif

//#if defined(EN_PLATFORM_WINDOWS)
//
//   // TODO: Needs to write it clean, without singleton
//   class cachealign NLogContext
//         {
//         private:
//         // Need to be declared to prevent from auto declaring
//         // them as public. That would allow copying operation
//         // on singleton.
//         NLogContext(const NLogContext& op); 
//         NLogContext& operator= (const NLogContext& op); 
//   
//         bool      m_initialized; // true  - Log file is open
//         bool      m_loging;      // Turns logging on or off
//         bool      m_tofile;      // true  - Logging to file
//                                  // false - Logging to console
//   	   fstream* m_output;       // Output file stream
//   
//         // Declaration of standard stream output function pointers
//         typedef std::basic_ostream<char, std::char_traits<char> > StreamOutType;
//         typedef StreamOutType& (*StreamFuncPtr)(StreamOutType&);
//   
//         forceinline void   init(void);                      // Opens log file
//   
//         public:
//         NLogContext();
//        ~NLogContext();
//   
//                     static NLogContext& getInstance(void);  // Returns reference to context
//         forceinline static NLogContext* getPointer(void);   // Returns pointer to context
//   
//         forceinline void  destination(uint8 dst);           // Sets loging destination
//         forceinline uint8 destination(void);                // Gets loging destination
//         forceinline void  on(void);                         // Turns loging on
//         forceinline void  off(void);                        // Turns loging off
//   
//         template <typename T>
//         NLogContext& operator << (const T& content);        // Accepts streams to log
//         NLogContext& operator << (StreamFuncPtr function);  // Accepts stream functions to log  
//         };
//
//   struct Interface : public NLogContext
//          {
//          };
//
//   typedef struct Interface Interface;
//#endif

   }

extern log::Interface Log;

#ifdef EN_DEBUG
      #define DebugLog( x ) Log << x;
#else
      #define DebugLog( x ) false /* Nothing in Release */
#endif
}

#include "core/log/log.inl"

#endif