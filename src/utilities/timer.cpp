/*

 Ngine v5.0
 
 Module      : Timer
 Requirements: types
 Description : Supports calculation of time on level
               of up to 1 microsecond.
   
*/

#include <stdlib.h>

#include "utilities/timer.h"

#ifdef EN_PLATFORM_BLACKBERRY
#include <time.h>
#endif

namespace en
{
   Time::Time() :
      dt(0)
   {
   }

   Time::Time(uint64 nanoseconds) :
      dt(nanoseconds)
   {
   }

   void Time::seconds(double time)
   {
   dt = static_cast<uint64>(time * 1000000000.0);
   }

   void Time::miliseconds(double time)
   {
   dt = static_cast<uint64>(time * 1000000.0);
   }

   void Time::miliseconds(uint64 time)
   {
   dt = time * 1000000;
   }

   void Time::microseconds(double time)
   {
   dt = static_cast<uint64>(time * 1000.0);
   }

   void Time::microseconds(uint64 time)
   {
   dt = time * 1000;
   }

   void Time::nanoseconds(double time)
   {
   dt = static_cast<uint64>(time);
   }

   void Time::nanoseconds(uint64 time)
   {
   dt = time;
   }

   double Time::seconds(void)
   {
   return static_cast<double>(dt) / 1000000000.0;
   }

   uint64 Time::miliseconds(void)
   {
   return dt / 1000000;
   }

   uint64 Time::microseconds(void)
   {
   return dt / 1000;
   }

   uint64 Time::nanoseconds(void)
   {
   return dt;
   }

   bool Time::operator== (Time right)
   {
   return dt == right.dt;
   }

   bool Time::operator!= (Time right)
   {
   return dt != right.dt;
   }
   bool Time::operator< (Time right)
   {
   return dt < right.dt;
   }

   bool Time::operator> (Time right)
   {
   return dt > right.dt;
   }

   bool Time::operator<= (Time right)
   {
   return dt <= right.dt;
   }

   bool Time::operator>= (Time right)
   {
   return dt >= right.dt;
   }

   Time Time::operator- (Time right)
   {
   return Time(dt - right.dt);
   }

   Time Time::operator+ (Time right)
   {
   return Time(dt + right.dt);
   }

   Time& Time::operator+= (Time right)
   {
   dt += right.dt;
   return *this;
   }

   Time& Time::operator-= (Time right)
   {
   dt -= right.dt;
   return *this;
   }

   Time Time::operator* (Time right)
   {
   return Time(dt * right.dt);
   }

   Time Time::operator* (uint64 right)
   {
   return Time(dt * right);
   }

   Time& Time::operator*= (Time right)
   {
   dt *= right.dt;
   return *this;
   }

   Time& Time::operator*= (uint64 right)
   {
   dt *= right;
   return *this;
   }





   Time Time::operator/ (Time right)
   {
   return Time(dt / right.dt);
   }

   Time Time::operator/ (uint64 right)
   {
   return Time(dt / right);
   }

   Time& Time::operator/= (Time right)
   {
   dt /= right.dt;
   return *this;
   }

   Time& Time::operator/= (uint64 right)
   {
   dt /= right;
   return *this;
   }

   void Timer::start(void)
   {
#if defined(EN_PLATFORM_ANDROID) || defined(EN_PLATFORM_BLACKBERRY) || defined(EN_PLATFORM_OSX)
   struct timespec now;
   clock_gettime(CLOCK_MONOTONIC, &now);
   time.nanoseconds( (static_cast<uint64>(now.tv_sec) * 1000000000LL) + static_cast<uint64>(now.tv_nsec) );
#endif
#ifdef EN_PLATFORM_IOS
   time.microseconds( static_cast<uint64>( [NSDate timeIntervalSinceReferenceDate] * 1000000.0 ) );
#endif
#ifdef EN_PLATFORM_WINDOWS
   LARGE_INTEGER frequency;
   LARGE_INTEGER offset;

   QueryPerformanceFrequency(&frequency);
   QueryPerformanceCounter(&offset);
   time.microseconds( static_cast<uint64>(offset.QuadPart * (1000000.0 / static_cast<double>(frequency.QuadPart))) );
#endif
   }

   Time Timer::elapsed(void)
   {
   Time newtime;

#if defined(EN_PLATFORM_ANDROID) || defined(EN_PLATFORM_BLACKBERRY) || defined(EN_PLATFORM_OSX)
   struct timespec now;
   clock_gettime(CLOCK_MONOTONIC, &now);
   newtime.nanoseconds( (static_cast<uint64>(now.tv_sec) * 1000000000LL) + static_cast<uint64>(now.tv_nsec) );
#endif
#ifdef EN_PLATFORM_IOS
   newtime.microseconds( static_cast<uint64>( [NSDate timeIntervalSinceReferenceDate] * 1000000.0 ) );
#endif
#ifdef EN_PLATFORM_WINDOWS
   LARGE_INTEGER frequency;
   LARGE_INTEGER offset;

   QueryPerformanceFrequency(&frequency);
   QueryPerformanceCounter(&offset);
   newtime.microseconds( static_cast<uint64>(offset.QuadPart * (1000000.0 / static_cast<double>(frequency.QuadPart))) );
#endif

   return newtime - time;
   }

}



// DEPRECATED BELOW :


// Constructor
Ntimer::Ntimer()
{
 this->t1 = 0.0;
 this->t2 = 0.0;

#ifdef EN_PLATFORM_WINDOWS
 QueryPerformanceFrequency(&this->freq);
 this->t1count.QuadPart = 0;
 this->t2count.QuadPart = 0;
#else
 this->t1count.tv_sec = this->t1count.tv_usec = 0;
 this->t2count.tv_sec = this->t2count.tv_usec = 0;
#endif
}

// Destructor
Ntimer::~Ntimer()
{
}

// Gets the start time
void Ntimer::start(void)
{
#ifdef EN_PLATFORM_ANDROID
struct timespec now;
clock_gettime(CLOCK_MONOTONIC, &now);
start.microseconds( (static_cast<uint64>(now.tv_sec) * 1000000000LL) + static_cast<uint64>(now.tv_nsec) );
#endif
#ifdef EN_PLATFORM_BLACKBERRY
gettimeofday(&t1count, NULL);
#endif
#ifdef EN_PLATFORM_IOS
t1 = [NSDate timeIntervalSinceReferenceDate] * 1000000.0;
#endif
#ifdef EN_PLATFORM_MACOS
gettimeofday(&t1count, NULL);
#endif
#ifdef EN_PLATFORM_WINDOWS
QueryPerformanceCounter(&t1count);
#endif
}

// Returns elapsed time till now
double Ntimer::elapsed(uint8 type = EN_SECONDS)
{
 double diff;

#ifdef EN_PLATFORM_BLACKBERRY
gettimeofday(&t2count, NULL);
t1 = (t1count.tv_sec * 1000000.0) + t1count.tv_usec;
t2 = (t2count.tv_sec * 1000000.0) + t2count.tv_usec;
#endif
#ifdef EN_PLATFORM_WINDOWS
 QueryPerformanceCounter(&this->t2count);
 t1 = this->t1count.QuadPart * (1000000.0 / freq.QuadPart);
 t2 = this->t2count.QuadPart * (1000000.0 / freq.QuadPart);
#endif
#ifdef ENG_MACOS
 gettimeofday(&this->t2count, NULL);
 t1 = (this->t1count.tv_sec * 1000000.0) + this->t1count.tv_usec;
 t2 = (this->t2count.tv_sec * 1000000.0) + this->t2count.tv_usec;
#endif
#ifdef ENG_IPHONE
 t2 = [NSDate timeIntervalSinceReferenceDate] * 1000000.0;
#endif
 diff = t2 - t1;

 if (type == EN_MILISECONDS)
    return diff * 0.001;

 if (type == EN_MICROSECONDS)
    return diff;

 return diff * 0.000001;
}