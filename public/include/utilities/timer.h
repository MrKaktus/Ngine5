/*

 Ngine v5.0
 
 Module      : Timer
 Requirements: types
 Description : Supports calculation of time on level
               of up to 1 microsecond.
   
*/

#ifndef ENG_UTILITIES_TIMER
#define ENG_UTILITIES_TIMER

#include "core/defines.h"
#include "core/types.h"

#ifdef EN_PLATFORM_WINDOWS
#include <windows.h>
#elif defined ENG_IPHONE
#import <QuartzCore/QuartzCore.h>
#else      // ENG_MACOS and other UNIX clone
#include <sys/time.h>
#endif


namespace en
{
   class Time
         {
         private:
         uint64 dt;

         public:
         Time();
         Time(uint64 nanoseconds);

         void seconds(double time);      
         void miliseconds(double time);
         void miliseconds(uint64 time);  
         void microseconds(double time);
         void microseconds(uint64 time);
         void nanoseconds(double time);
         void nanoseconds(uint64 time);

         double seconds(void);      
         uint64 miliseconds(void);  
         uint64 microseconds(void); 
         uint64 nanoseconds(void); 

         bool  operator== (Time right);
         bool  operator!= (Time right);
         bool  operator<  (Time right);
         bool  operator>  (Time right);
         bool  operator<= (Time right);
         bool  operator>= (Time right);
         Time  operator-  (Time right);
         Time  operator+  (Time right);
         Time& operator+= (Time right);
         Time& operator-= (Time right);
         Time  operator*  (Time   right);
         Time  operator*  (uint64 right);
         Time& operator*= (Time   right);
         Time& operator*= (uint64 right);
         Time  operator/  (Time   right);
         Time  operator/  (uint64 right);
         Time& operator/= (Time   right);
         Time& operator/= (uint64 right);
         };

   class Timer
         {
         private:
         Time time;

         public:
         void start(void);    // Starts counting time
         Time elapsed(void);  // Returns current elapsed time
         };
}


//
//namespace en
//{
//   namespace time
//   {
//   // Time unit
//   enum Interval
//        {
//        Seconds     = 0,
//        Miliseconds    ,
//        Microseconds   
//        };
//
//   }
//}

#define EN_SECONDS      1
#define EN_MILISECONDS  2
#define EN_MICROSECONDS 3

class Ntimer
      {
      private:
      double t1;
      double t2;

     #ifdef EN_PLATFORM_WINDOWS              // Counters for Windows
      LARGE_INTEGER freq;                    
      LARGE_INTEGER t1count;                   
      LARGE_INTEGER t2count; 
     #else
      timeval t1count;               // Counters for Unix and Linux                  
      timeval t2count;                           
     #endif

      public:
      Ntimer();
     ~Ntimer();
  
      void   start(void);            // Starts to count time
      double elapsed(uint8 type);    // Returns elapsed time from start
      };

#endif
