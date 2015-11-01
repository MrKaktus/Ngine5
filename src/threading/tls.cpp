/*

 Ngine v5.0
 
 Module      : Thread Local Storage.
 Visibility  : Engine only.
 Requirements: none
 Description : Delivers easy interface for
               handling Thread Local Storage.

*/


#include "threading/context.h"

namespace en
{
   namespace threads
   {

   TLS::TLS() :
      created(false)
#ifdef EN_PLATFORM_WINDOWS
      , index(TLS_OUT_OF_INDEXES)
#endif
   {
   create();
   }
   
   TLS::~TLS()
   {
   destroy();
   }
   
   bool TLS::create(void)
   {
   if (!created)
      {
#ifdef EN_PLATFORM_WINDOWS
      index = TlsAlloc();
      if (index != TLS_OUT_OF_INDEXES)
         created = true;
#endif
      }
   
   return created;
   }
   
   void TLS::destroy(void)
   {
   if (created)
      {
#ifdef EN_PLATFORM_WINDOWS
      TlsFree(index);
      created = false;
#endif
      }
   }
   
   void TLS::set(void* ptr)
   {
   assert( created );

#ifdef EN_PLATFORM_WINDOWS
   TlsSetValue(index, ptr);
#else
   assert(0);
#endif
   }

   void* TLS::get(void)
   {
   assert( created );

#ifdef EN_PLATFORM_WINDOWS
   return TlsGetValue(index);
#else
   assert(0);
   return nullptr;
#endif
   }

   }
}







//
//
//#include "Ngine4/threading/Ntls.h"
//
//Ntls::Ntls() :
//      m_created(false)
//#ifdef EN_PLATFORM_WINDOWS
//    , m_index(TLS_OUT_OF_INDEXES)
//#endif
//{
// create();
//}
//
//Ntls::~Ntls()
//{
// destroy();
//}
//
//// Creates Thread Local Storage
//bool Ntls::create(void)
//{
// if (!m_created)
//    {
//#ifdef EN_PLATFORM_WINDOWS
//    m_index = TlsAlloc();
//    if (m_index != TLS_OUT_OF_INDEXES)
//       m_created = true;
//#endif
//    }
//
// return m_created;
//}
//
//// Destroys Thread Local Storage
//void Ntls::destroy(void)
//{
// if (m_created)
//    {
//#ifdef EN_PLATFORM_WINDOWS
//    TlsFree(m_index);
//    m_created = false;
//#endif
//    }
//}
//
//// Sets pointer to Thread-dependent data
//void Ntls::set(void* ptr)
//{
// if (m_created)
//    {
//#ifdef EN_PLATFORM_WINDOWS
//    TlsSetValue(m_index, ptr);
//#endif
//    }
//}
//
//// Gets pointer to Thread-dependent data
//void* Ntls::get(void)
//{
// if (m_created)
//    {
//#ifdef EN_PLATFORM_WINDOWS
//    return TlsGetValue(m_index);
//#endif
//    }
// return 0;
//}