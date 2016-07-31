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

// TODO: In future replace all that stuff with C++11 shared_ptr & weak_ptr
//       when ensured that they are properly supported on all platforms
//       (and that they don't suck mem/perf/cache more than they need to)

namespace en
{
   template <typename T>
   class Weak;
   
   // Strong Pointer:
   //
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
      Ptr(const Weak<T>& src);          // Copy constructor
      Ptr& operator=(const Ptr& src);   // Copy Assigment operator
     ~Ptr();                            // Destructor

      // Check at the beginning, if pointer is properly initiated
      // with objects adress. If it is, you are guaranteed that it
      // will be always proper and you can safely use the object
      // pointed by it.
      operator bool() const; 
      T* operator->() const;            // Pointer operator, allows safe accessing of object. 
      bool operator== (Ptr<T> b) const; // Comparison operator

      const T& operator*() const;       // Indirection operator return const reference of object
      
      // Returns count of references to this object.
      uint32 references(void);
      };
 
   template <typename T>
   class cachealign SafeObject;
   
   // Weak Pointer:
   //
   // Class that implements objects management using
   // Thread-Safe intrusive pointers. It should be
   // inherited as public.
   template <typename T>
   class Weak
      {
      friend class Ptr<T>;               // Ptr<T> can extract pointer from Weak<T> on it's creation
      friend class SafeObject<T>;        // Object at which Weak pointer points, can detach it automatically
      
      // Ptr protects pointer to proper data
      // which means that only class that inherits from
      // it and implements interface methods can access
      // that pointer.
      protected:
      T* pointer;                        // Pointer to managed data
      Weak* prev;                        // Previous Weak pointer to this instance
      Weak* next;                        // Next Weak pointer to this instance

      public:
      Weak();                            // Default Constructor
      Weak(T* src);                      // Constructor
      Weak(const Weak<T>& src);          // Copy constructor
      Weak& operator=(const Weak& src);  // Copy Assigment operator
     ~Weak();                            // Destructor

      // Check at the beginning, if pointer is properly initiated
      // with objects adress. If it is, you are guaranteed that it
      // will be always proper and you can safely use the object
      // pointed by it.
      operator bool() const; 
      T* operator->() const;             // Pointer operator, allows safe accessing of object.
      bool operator== (Weak<T> b) const; // Comparison operator

      const T& operator*() const;        // Indirection operator return const reference of object
      
      // Returns count of references to this object.
      uint32 references(void);
      };
   
   // Base class that implements objects management
   // using Thread-Safe intrusive pointers.
   template <typename T>
   class cachealign SafeObject
      {
      public:
      volatile uint32   references;
      volatile Weak<T>* head;        // Weak pointers queue head
      
      SafeObject() :
         references(0),
         head(nullptr)
      {
      };
      
      virtual ~SafeObject()
      {
      // Iterate over list of weak pointers, and nullify them
      while(head != nullptr)
         {
         Weak<T>* temp = head->next;
         
         // Detach Weak pointer
         head->pointer = nullptr;
         head->prev    = nullptr;
         head->next    = nullptr;
         
         head = temp;
         }
      };

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
      AtomicIncrease(&((SafeObject<T>*)pointer)->references);
   }

   template <typename T>
   Ptr<T>::Ptr(const Ptr<T>& src) :
      pointer(src.pointer)
   {
   if (pointer)
      AtomicIncrease(&((SafeObject<T>*)pointer)->references);
   }

   template <typename T>
   Ptr<T>::Ptr(const Weak<T>& src) :
      pointer(src.pointer)
   {
   // This is not Thread-Safe. Between assigment from Weak, checking ptr, and Atomic operation it may be discarded.
   if (pointer)
      AtomicIncrease(&((SafeObject<T>*)pointer)->references);
   }
   
   template <typename T>
   Ptr<T>& Ptr<T>::operator=(const Ptr<T>& src)
   {
   if (pointer == src.pointer)
      return *this;

   // Stops being interface of old value
   if (pointer)
      {
      uint32 ref = AtomicDecrease(&((SafeObject<T>*)pointer)->references);
      if (ref == 0)
         delete pointer;
      }

   // Starts to be interface of new value
   pointer = src.pointer;
   if (pointer)
      AtomicIncrease(&((SafeObject<T>*)pointer)->references);
 
   return *this;
   }

   template <typename T>
   Ptr<T>::~Ptr()
   {
   if (pointer)
      {
      uint32 ref = AtomicDecrease(&((SafeObject<T>*)pointer)->references);
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

   // const version, returns a const reference instead of just a reference
   // to enforce the idea of the logical constness of this object
   template <typename T>
   const T& Ptr<T>::operator*() const
   {
   return *pointer;
   }

   template <typename T>
   bool Ptr<T>::operator== (Ptr<T> b) const
   {
   return this->pointer == b.pointer;
   }
  
   template <typename T>
   uint32 Ptr<T>::references(void)
   {
   return pointer == nullptr ? 0 : ((SafeObject<T>*)pointer)->references;
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
   
   
   
   // WEAK POINTER
   
   template <typename T>
   Weak<T>::Weak() :
      pointer(nullptr),
      prev(nullptr),
      next(nullptr)
   {
   }
   
   template <typename T>
   Weak<T>::Weak(T* src) :
      pointer(src),
      prev(nullptr),
      next(nullptr)
   {
   if (pointer)
      {
      // This weak element is added as new head of the list
      next = static_cast<void*>(AtomicSwap(&((SafeObject<T>*)pointer)->head, this));
      if (next)
         next->prev = this;
      }
   }

   template <typename T>
   Weak<T>::Weak(const Weak<T>& src) :
      pointer(src.pointer),
      prev(nullptr),
      next(nullptr)
   {
   if (pointer)
      {
      // This weak element is added as new head of the list
      next = static_cast<void*>(AtomicSwap(&((SafeObject<T>*)pointer)->head, this));
      if (next)
         next->prev = this;
      }
   }

   template <typename T>
   Weak<T>& Weak<T>::operator=(const Weak<T>& src)
   {
   if (pointer == src.pointer)
      return *this;

   // Stops being interface of old value
   if (pointer)
      {
      if (prev)
         AtomicSwap(&prev->next, next);
      if (next)
         AtomicSwap(&next->prev, prev);
      }

   // Starts to be interface of new value
   pointer = src.pointer;
   if (pointer)
      {
      // This weak element is added as new head of the list
      next = static_cast<void*>(AtomicSwap(&((SafeObject<T>*)pointer)->head, this));
      if (next)
         next->prev = this;
      }
      
   return *this;
   }


   template <typename T>
   Weak<T>::~Weak()
   {
   // Stops being interface of old value
   if (pointer)
      {
      // TODO: !!! Cast to 32bit equivalents on 32bit system if using AtomicSwap !!!
      // TODO: ??? Do we need to use AtomicSwap at all ???
      if (prev)
         prev->next = next;
         //AtomicSwap(reinterpret_cast<volatile uint64*>(&prev->next), reinterpret_cast<uint64>(next));
      if (next)
         next->prev = prev;
         //AtomicSwap(reinterpret_cast<volatile uint64*>(&next->prev), reinterpret_cast<uint64>(prev));
      }
   }

   template <typename T>
   Weak<T>::operator bool() const
   {
   return pointer != nullptr ? true : false;
   }

   template <typename T>
   T* Weak<T>::operator->() const
   {
   return pointer;
   }
   
   // const version, returns a const reference instead of just a reference
   // to enforce the idea of the logical constness of this object
   template <typename T>
   const T& Weak<T>::operator*() const
   {
   return *pointer;
   }

   template <typename T>
   bool Weak<T>::operator== (Weak<T> b) const
   {
   return this->pointer == b.pointer;
   }
  
   template <typename T>
   uint32 Weak<T>::references(void)
   {
   return pointer == nullptr ? 0 : ((SafeObject<T>*)pointer)->references;
   }

   template <typename T, typename Other>
   Weak<T> weak_dynamic_cast(const Weak<Other>& src)
   {
   T* ptr = dynamic_cast<T*>( *((Other**)(&src)) );
   return Weak<T>(ptr);
   }


}

#endif
