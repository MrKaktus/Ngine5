/*

 Ngine v5.0
 
 Module      : Proxy interface.
 Visibility  : Full version only.
 Requirements: none
 Description : Supports programmer with implementation
               of thread safe intrusive proxy interface.

*/

#ifndef ENG_CORE_UTILITIES_PROXY
#define ENG_CORE_UTILITIES_PROXY

#include "core/defines.h"
#include "core/types.h"
#include "core/utilities/aligment.h"
#include "core/threading/atomics.h"

// Base of class that implements object managed
// by proxy interface in Thread-Safe manner.
class cachealign ProxyObject
      {
      public:
      volatile uint32 references;

      ProxyObject() :
          references(0)
          {
          };
      };


// Base of class that implements proxy interface
// to manage objects in Thread-Safe manner. It
// should be inherited as public.
template <typename T>
class ProxyInterface
      {
      // ProxyInterface protects pointer to proper data
      // which means that only class that inherits from
      // it and implements interface methods can access
      // that pointer.
      protected:
      T* pointer;                       // Pointer to managed data
      static bool (*destroy)(T* const); // Pointer to function that destroys data.
                                        // This pointer needs to be instantiated in
                                        // corresponding cpp file, like this:
                                        // template<> bool (*ProxyInterface<T>::destroy)(T* const) = <here func ptr!>;

      public:
      ProxyInterface(T* src);                               // Constructor
      ProxyInterface(const ProxyInterface& src);            // Copy constructor
      ProxyInterface& operator=(const ProxyInterface& src); // Copy Assigment operator
     ~ProxyInterface();                                     // Destructor

      // First instance of interface, that is created when 
      // pointer to managed data is passed, should be always
      // checked to ensure that creation of that data was 
      // successfull. After this check this interface and 
      // all copies of it will be always safe to use. 
      operator bool() const; 

      bool operator== (ProxyInterface<T> b) const;          // Comparison operator

      // Returns count of references to this object.
      uint32 references(void);
      };



template <typename T>
ProxyInterface<T>::ProxyInterface(T* src) :
    pointer(src)
{
 if (pointer)
    AtomicIncrease(&((ProxyObject*)pointer)->references);
}

template <typename T>
ProxyInterface<T>::ProxyInterface(const ProxyInterface& src) :
    pointer(src.pointer)
{
 if (pointer)
    AtomicIncrease(&((ProxyObject*)pointer)->references);
}

template <typename T>
ProxyInterface<T>& ProxyInterface<T>::operator=(const ProxyInterface<T>& src)
{
 if (pointer == src.pointer)
    return *this;

 // Stops being interface of old value
 if (pointer)
    {
    uint32 ref = AtomicDecrease(&((ProxyObject*)pointer)->references);
    if (ref == 0)
       (*destroy)(pointer);
    }

 // Starts to be interface of new value
 pointer = src.pointer;
 if (pointer)
    AtomicIncrease(&((ProxyObject*)pointer)->references);
 
 return *this;
}

template <typename T>
ProxyInterface<T>::~ProxyInterface()
{
 if (pointer)
    {
    uint32 ref = AtomicDecrease(&((ProxyObject*)pointer)->references);
    if (ref == 0)
       (*destroy)(pointer);
    }
}

template <typename T>
ProxyInterface<T>::operator bool() const
{
 return pointer != 0 ? true : false;
}

template <typename T>
bool ProxyInterface<T>::operator== (ProxyInterface<T> b) const
{
 return this->pointer == b.pointer;
}
  
template <typename T>
uint32 ProxyInterface<T>::references(void)
{
 return pointer == 0 ? 0 : ((ProxyObject*)pointer)->references;
}

#endif