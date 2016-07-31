/*

 Ngine v5.0
 
 Module      : Version description.
 Visibility  : All versions.
 Requirements: none
 Description : Declares structure which allows easy
               storage and comparision of different
               versions. 

*/

#ifndef ENG_UTILITIES_VERSION
#define ENG_UTILITIES_VERSION

#include "core/defines.h"
#include "core/types.h"

union Nversion
      {
      struct { 
             uint32 release : 8; // API release number
             uint32 api     : 4; // API identified
             uint32 low     : 4;
             uint32 medium  : 8;
             uint32 high    : 8;
             };
      uint32 value;

      forceinline Nversion();
      forceinline Nversion(uint32 val);
      forceinline Nversion(uint32 _release, uint32 _api, uint32 _high, uint32 _medium, uint32 _low);
      forceinline bool better(const Nversion version) const; // True if current version is better or equal to specified one
      };

forceinline bool operator ==(const Nversion& a, const Nversion& b)
{
 return (a.value == b.value);
}

forceinline Nversion::Nversion() :
            value(0)
{
}

forceinline Nversion::Nversion(uint32 val) :
            value(val)
{
}

forceinline Nversion::Nversion(uint32 _release, uint32 _api, uint32 _high, uint32 _medium, uint32 _low) :
            release(_release),
            api(_api),
            low(_low),
            medium(_medium),
            high(_high)
{
}

// Returns true if better or equal
forceinline bool Nversion::better(const Nversion version) const
{
 if (api != version.api)
    return false;

 if (high < version.high)
    return false;
 if (high > version.high)
    return true;

 // High versions are identical
 if (medium < version.medium)
    return false;
 if (medium > version.medium)
    return true;

 // Medium versions are identical
 if (low < version.low)
    return false;
 else
    return true;
}

#endif
