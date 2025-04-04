/*

 Ngine v5.0
 
 Module      : Non Copyable class.
 Requirements: none
 Description : This class prevents creation of copies of
               each instance of class that inherits from it.
               It is useful for eg. for objects that should 
               be managed in safe way through intrusive pointers.

*/

#ifndef ENG_CORE_UTILITIES_NONCOPYABLE
#define ENG_CORE_UTILITIES_NONCOPYABLE

#include "core/defines.h"

class NonCopyable
{
    // Constructor is protected to prevent creation of other 
    // copies and to allow child class to perform call to it 
    // during construction at the same time.
    protected:
    NonCopyable(){}
   
    // Copy constructor and assigment operator are directly 
    // declared as private to prevent creation of copies.
    private:
    NonCopyable(const NonCopyable&){}
#ifndef EN_PLATFORM_OSX
    NonCopyable& operator= (const NonCopyable&){}
#endif
};

#endif