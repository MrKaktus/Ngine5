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

// In Debug mode logging is enabled by default to console window.
// In Release mode logging is disabled by default.
// It can be enabled from config file, to log to log file.
class Interface 
{
public:

    // Declaration of standard stream output function pointers
    typedef std::basic_ostream<char, std::char_traits<char> > StreamOutType;
    typedef StreamOutType& (*StreamFuncPtr)(StreamOutType&);

    /// Creates this class instance and assigns it to "Log"
    static bool create(void);

    virtual bool        destination(Destination dst) = 0;      // Sets loging destination (can fail if log file cannot be opened)
    virtual Destination destination(void) = 0;                 // Gets loging destination
    virtual void        on(void) = 0;                          // Turns loging on
    virtual void        off(void) = 0;                         // Turns loging off 

    // Stream style logging

    virtual Interface& operator << (const uint8& in) = 0;
    virtual Interface& operator << (const uint16& in) = 0;
    virtual Interface& operator << (const uint32& in) = 0;
    virtual Interface& operator << (const uint64& in) = 0;
    virtual Interface& operator << (const sint8& in) = 0;
    virtual Interface& operator << (const sint16& in) = 0;
    virtual Interface& operator << (const sint32& in) = 0;
    virtual Interface& operator << (const sint64& in) = 0;
    virtual Interface& operator << (const float& in) = 0;
    virtual Interface& operator << (const double& in) = 0;
    virtual Interface& operator << (const char in) = 0;
    virtual Interface& operator << (const char* in) = 0;
    virtual Interface& operator << (const std::string in) = 0;

    template <typename T>
    Interface& operator << (const T& content);                    // Accepts streams to log
    virtual Interface& operator << (StreamFuncPtr function) = 0;  // Accepts stream functions to log

    // Print style logging

    virtual void operator()(const char* format, ...) = 0;
    virtual void error(const char* format, ...) = 0;
};

template <typename T>
Interface& Interface::operator << (const T& content)
{
    std::cout << content;
    return *this;
}

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

} // en::log

extern std::unique_ptr<log::Interface> Log;

} // en

#define enLog (*en::Log)

#ifdef EN_DEBUG
#define logDebug( x ) enLog << x;
#else
#define logDebug( x ) false /* Nothing in Release */
#endif


#include "core/log/log.inl"

#endif
