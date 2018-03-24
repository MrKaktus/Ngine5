/*

 Ngine v5.0
 
 Module      : Timer
 Requirements: types
 Description : Supports calculation of time on level
               of up to 1 nanosecond.
   
*/

#ifndef ENG_UTILITIES_TIMER
#define ENG_UTILITIES_TIMER

#include "core/defines.h"
#include "core/types.h"

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
   
   Time currentTime(void);
   void sleepUntil(Time time);
}

#endif
