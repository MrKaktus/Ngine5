/*

 Ngine v5.0
 
 Module      : Intrusive pointer.
 Requirements: none
 Description : Intrusive pointer implementation that allows 
               Thread-Safe management of objects.

*/

#ifndef ENG_CORE_UTILITIES_TINTRUSIVEPOINTER
#define ENG_CORE_UTILITIES_TINTRUSIVEPOINTER

#include "core/defines.h"
#include "core/utilities/aligment.h"
#include "core/threading/atomics.h"

namespace en
{
   // Base class that implements objects management
   // using Thread-Safe intrusive pointers.
   class cachealign SafeObject
      {
      public:
      volatile uint32 references;

      SafeObject() :
         references(0)
      {
      };

      };

   // Class that implements objects management using
   // Thread-Safe intrusive pointers. It should be
   // inherited as public.
   template <typename T>
   class Ptr
      {
      // Ptr protects pointer to proper data
      // which means that only class that inherits from
      // it and implements interface methods can access
      // that pointer.
      protected:
      T* pointer;                       // Pointer to managed data

      public:
      Ptr();                            // Default Constructor
      Ptr(T* src);                      // Constructor
      Ptr(const Ptr<T>& src);           // Copy constructor
      Ptr& operator=(const Ptr& src);   // Copy Assigment operator
     ~Ptr();                            // Destructor

      // Check at the beginning, if pointer is properly initiated
      // with objects adress. If it is, you are guaranteed that it
      // will be always proper and you can safely use the object
      // pointed by it.
      operator bool() const; 
      T* operator->() const;            // Pointer operator, allows safe accessing of object. 
      bool operator== (Ptr<T> b) const; // Comparison operator

      // Returns count of references to this object.
      uint32 references(void);
      };

   template <typename T>
   Ptr<T>::Ptr() :
      pointer(nullptr)
   {
   }

   template <typename T>
   Ptr<T>::Ptr(T* src) :
      pointer(src)
   {
   if (pointer)
      AtomicIncrease(&((SafeObject*)pointer)->references);
   }

   template <typename T>
   Ptr<T>::Ptr(const Ptr<T>& src) :
      pointer(src.pointer)
   {
   if (pointer)
      AtomicIncrease(&((SafeObject*)pointer)->references);
   }

   template <typename T>
   Ptr<T>& Ptr<T>::operator=(const Ptr<T>& src)
   {
   if (pointer == src.pointer)
      return *this;

   // Stops being interface of old value
   if (pointer)
      {
      uint32 ref = AtomicDecrease(&((SafeObject*)pointer)->references);
      if (ref == 0)
         delete pointer;
      }

   // Starts to be interface of new value
   pointer = src.pointer;
   if (pointer)
      AtomicIncrease(&((SafeObject*)pointer)->references);
 
   return *this;
   }

   template <typename T>
   Ptr<T>::~Ptr()
   {
   if (pointer)
      {
      uint32 ref = AtomicDecrease(&((SafeObject*)pointer)->references);
      if (ref == 0)
         delete pointer;
      }
   }

   template <typename T>
   Ptr<T>::operator bool() const
   {
   return pointer != nullptr ? true : false;
   }

   template <typename T>
   T* Ptr<T>::operator->() const
   {
   return pointer;
   }

   //template <typename T>
   //Ptr<T>::operator&() const
   //{
   //}
   //
   //template <typename T>
   //T Ptr<T>::operator*() const
   //{
   //}

   template <typename T>
   bool Ptr<T>::operator== (Ptr<T> b) const
   {
   return this->pointer == b.pointer;
   }
  
   template <typename T>
   uint32 Ptr<T>::references(void)
   {
   return pointer == nullptr ? 0 : ((SafeObject*)pointer)->references;
   }

   template <typename T, typename Other>
   Ptr<T> ptr_dynamic_cast(const Ptr<Other>& src)
   {
   T* ptr = dynamic_cast<T*>( *((Other**)(&src)) );
   return Ptr<T>(ptr);
   }

   //
   //// Dynamic cast intrusive pointer types
   //template <typename T, typename Tsrc>
   //T dynamic_cast_ptr(Ptr<Tsrc>)
   //{
   //return *(Ptr<T>*)(&Ptr<Tsrc>);
   //}
   //
   //template <class Ty, class Other>
   //Ptr<Ty> dynamic_pointer_cast(const Ptr<Other>& sp)
   //{
   //return
   //}
}

#endif