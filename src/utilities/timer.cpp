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

#ifdef EN_PLATFORM_IOS
#import <QuartzCore/QuartzCore.h>
#endif

#if defined(EN_PLATFORM_OSX)
//#include <sys/time.h>
#ifdef aligned
#undef aligned
#endif
#include <mach/mach.h>
#include <mach/mach_time.h>

static mach_timebase_info_data_t timebase = { 0, 0 };
#endif

#if defined(EN_PLATFORM_WINDOWS)
// More about High-Resolution Time Stamps:
// https://msdn.microsoft.com/en-us/library/windows/desktop/dn553408(v=vs.85).aspx
//
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <timeapi.h>
#include "assert.h"
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

   double Time::seconds(void) const
   {
   return static_cast<double>(dt) / 1000000000.0;
   }

   uint64 Time::miliseconds(void) const
   {
   return dt / 1000000;
   }

   uint64 Time::microseconds(void) const
   {
   return dt / 1000;
   }

   uint64 Time::nanoseconds(void) const
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
   time = currentTime();
   }

   Time Timer::elapsed(void)
   {
   return currentTime() - time;
   }

   Time currentTime(void)
   {
   Time current;
   
#if defined(EN_PLATFORM_ANDROID) || defined(EN_PLATFORM_BLACKBERRY)
   struct timespec now;
   clock_gettime(CLOCK_MONOTONIC, &now);
   current.nanoseconds( (static_cast<uint64>(now.tv_sec) * 1000000000LL) + static_cast<uint64>(now.tv_nsec) );
#endif
#if defined(EN_PLATFORM_IOS)
   current.microseconds( static_cast<uint64>( [NSDate timeIntervalSinceReferenceDate] * 1000000.0 ) );
#endif
#if defined(EN_PLATFORM_OSX)
   if (timebase.denom == 0 )
      mach_timebase_info(&timebase);
   current.nanoseconds( mach_absolute_time() * timebase.numer / timebase.denom );
#endif
#if defined(EN_PLATFORM_WINDOWS)
   LARGE_INTEGER frequency;
   LARGE_INTEGER offset;

   QueryPerformanceFrequency(&frequency);
   QueryPerformanceCounter(&offset);
   current.microseconds( static_cast<uint64>(offset.QuadPart * (1000000.0 / static_cast<double>(frequency.QuadPart))) );
#endif

   return current;
   }

   void sleepFor(Time time)
   {
#if defined(EN_PLATFORM_OSX)
   if (timebase.denom == 0 )
      mach_timebase_info(&timebase);
   uint64 machAbsoluteTime = mach_absolute_time() + (time.nanoseconds() * timebase.denom) / timebase.numer;
   mach_wait_until(machAbsoluteTime);
#else
   // TODO: Windows
   assert( 0 );
#endif
   }

   void sleepUntil(Time time)
   {
#if defined(EN_PLATFORM_OSX)
   if (timebase.denom == 0 )
      mach_timebase_info(&timebase);
   uint64 machAbsoluteTime = (time.nanoseconds() * timebase.denom) / timebase.numer;
   mach_wait_until(machAbsoluteTime);
#else
   // TODO: Windows 

   // Multimedia timers provide higher resolution than 1ms:
   // https://msdn.microsoft.com/en-us/library/windows/desktop/dd743609(v=vs.85).aspx

   // Very detailed timer resolution measurements:
   // http://www.windowstimestamp.com/description

   // Query Timer Resolution (in miliseconds, so not precise enough unfortunately)
   // https://msdn.microsoft.com/en-us/library/windows/desktop/dd757633(v=vs.85).aspx
   TIMECAPS timerCapabilities;
   MMRESULT result = timeGetDevCaps(&timerCapabilities, sizeof(timerCapabilities));
   if (result != MMSYSERR_NOERROR)
      {
      // TODO: Log Error message!
      }

   // Increase Timer Resolution to maximum
   // https://msdn.microsoft.com/en-us/library/dd757624(v=vs.85).aspx
   //
   UINT targetResolution = 1; // 1ms
   UINT timerResolution = min(max(timerCapabilities.wPeriodMin, targetResolution), timerCapabilities.wPeriodMax);
   result = timeBeginPeriod(timerResolution);

   // TODO: Wait thread here

   result = timeEndPeriod(timerResolution);
   assert( 0 );
#endif
   }
}

/*

TODO: Consider completly separate implementation for macOS that stores time as native mach time instead of in nanoseconds

#if defined(EN_PLATFORM_OSX)

#define machToSeconds(x)      (((double)(x) / 1000000000.0) * (double(timebase.numer) / double(timebase.denom)))
#define machToMiliSeconds(x)  (((double)(x) / 1000000.0) * (double(timebase.numer) / double(timebase.denom)))
#define machToMicroSeconds(x) (((double)(x) / 1000.0) * (double(timebase.numer) / double(timebase.denom)))
#define machToNanoSeconds(x)  ((double)(x) * (double(timebase.numer) / double(timebase.denom)))

Time::Time() :
    value(0)
{
    if (timebase.denom == 0)
        mach_timebase_info(&timebase);
}

Time::Time(double time)
{
    if (timebase.denom == 0)
        mach_timebase_info(&timebase);
   
    value = uint64(time * 1000000000.0 * ( double(timebase.denom) / double(timebase.numer) ));
}


Time Time::now(void)
{
    XRTime time;
    time.value = mach_absolute_time();
   
    return time;
}

Time Time::delta(Time past)
{
    XRTime time;
    time.value = mach_absolute_time() - past.value;
   
    return time;
}

double Time::seconds(void)
{
    return machToSeconds(value);
}

double Time::miliseconds(void)
{
    return machToMiliSeconds(value);
}

double Time::microseconds(void)
{
    return machToMicroSeconds(value);
}

double Time::nanoseconds(void)
{
    return machToNanoSeconds(value);
}

void Time::seconds(double time)
{
    value = uint64(time * 1000000000.0 * ( double(timebase.denom) / double(timebase.numer) ));
}

#endif
//*/



// DEPRECATED BELOW :
//
// // Constructor
// Ntimer::Ntimer()
// {
//  this->t1 = 0.0;
//  this->t2 = 0.0;
// 
// #ifdef EN_PLATFORM_WINDOWS
//  QueryPerformanceFrequency(&this->freq);
//  this->t1count.QuadPart = 0;
//  this->t2count.QuadPart = 0;
// #else
//  this->t1count.tv_sec = this->t1count.tv_usec = 0;
//  this->t2count.tv_sec = this->t2count.tv_usec = 0;
// #endif
// }
// 
// // Destructor
// Ntimer::~Ntimer()
// {
// }
// 
// // Gets the start time
// void Ntimer::start(void)
// {
// #ifdef EN_PLATFORM_ANDROID
// struct timespec now;
// clock_gettime(CLOCK_MONOTONIC, &now);
// start.microseconds( (static_cast<uint64>(now.tv_sec) * 1000000000LL) + static_cast<uint64>(now.tv_nsec) );
// #endif
// #ifdef EN_PLATFORM_BLACKBERRY
// gettimeofday(&t1count, NULL);
// #endif
// #ifdef EN_PLATFORM_IOS
// t1 = [NSDate timeIntervalSinceReferenceDate] * 1000000.0;
// #endif
// #ifdef EN_PLATFORM_MACOS
// gettimeofday(&t1count, NULL);
// #endif
// #ifdef EN_PLATFORM_WINDOWS
// QueryPerformanceCounter(&t1count);
// #endif
// }
// 
// // Returns elapsed time till now
// double Ntimer::elapsed(uint8 type = EN_SECONDS)
// {
//  double diff;
// 
// #ifdef EN_PLATFORM_BLACKBERRY
// gettimeofday(&t2count, NULL);
// t1 = (t1count.tv_sec * 1000000.0) + t1count.tv_usec;
// t2 = (t2count.tv_sec * 1000000.0) + t2count.tv_usec;
// #endif
// #ifdef EN_PLATFORM_WINDOWS
//  QueryPerformanceCounter(&this->t2count);
//  t1 = this->t1count.QuadPart * (1000000.0 / freq.QuadPart);
//  t2 = this->t2count.QuadPart * (1000000.0 / freq.QuadPart);
// #endif
// #ifdef ENG_MACOS
//  gettimeofday(&this->t2count, NULL);
//  t1 = (this->t1count.tv_sec * 1000000.0) + this->t1count.tv_usec;
//  t2 = (this->t2count.tv_sec * 1000000.0) + this->t2count.tv_usec;
// #endif
// #ifdef ENG_IPHONE
//  t2 = [NSDate timeIntervalSinceReferenceDate] * 1000000.0;
// #endif
//  diff = t2 - t1;
// 
//  if (type == EN_MILISECONDS)
//     return diff * 0.001;
// 
//  if (type == EN_MICROSECONDS)
//     return diff;
// 
//  return diff * 0.000001;
// }
