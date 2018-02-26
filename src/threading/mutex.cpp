/*

 Ngine v5.0
 
 Module      : Threading primitive.
 Requirements: none
 Description : Supports programmer with platform independent
               implementation of mutex threading primitive.

*/

#include "threading/mutex.h"

namespace en
{
   // By defaul lock is free
   Mutex::Mutex() :
      lockValue(0)
   {
   }
   
   // Automatically unlocks during destruction
   Mutex::~Mutex() 
   {
   unlock();
   }
}