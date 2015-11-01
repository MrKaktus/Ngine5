/*

 Ngine v5.0
 
 Module      : Threading primitive.
 Visibility  : Full version only.
 Requirements: none
 Description : Supports programmer with platform independent
               implementation of mutex threading primitive.

*/

#include "threading/mutex.h"

// As defaul lock is free
Nmutex::Nmutex() :
   m_lock(0)
{
}

// Automatically unlocks during destruction
Nmutex::~Nmutex() 
{
unlock();
}