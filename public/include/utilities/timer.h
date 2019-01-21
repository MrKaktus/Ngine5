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
      uint64 dt;  // In nanoseconds

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

      double seconds(void) const;
      uint64 miliseconds(void) const;
      uint64 microseconds(void) const;
      uint64 nanoseconds(void) const;

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
    
   static_assert(sizeof(Time) == 8, "en::Time size mismatch!");

   class Timer
      {
      private:
      Time time;

      public:
      void start(void);    // Starts counting time (or restarts if already running)
      Time elapsed(void);  // Returns current elapsed time
      };
   
   static_assert(sizeof(Timer) == 8, "en::Timer size mismatch!");

   Time currentTime(void);
   
   void sleepFor(Time time);   ///< Sleeps for given period of time
   void sleepUntil(Time time); ///< Slepps until given absolute moment in time
}

#endif
